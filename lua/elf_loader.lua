local MAPPING_ADDR        = 0x926100000
local SHADOW_MAPPING_ADDR = 0x920100000

local PROT_RW  = 0x3
local PROT_RWX = 0x7
local MAP_SHARED_FIXED       = 0x11
local MAP_ANON_PRIVATE_FIXED = 0x1012

local IPPROTO_IPV6        = 41
local IPV6_PKTINFO        = 46
local AF_INET6    = 28
local SOCK_DGRAM  = 2
local IPPROTO_UDP = 17

-- error handling

local function fail(msg)
    ulog("[elf] ERROR: " .. msg)
    send_notification(msg)
    return nil
end

-- helpers

local function get_ofiles()
    local p_fd = kread64(curproc + OFF.PROC_FD)
    local tbl  = kread64(p_fd + OFF.FILEDESC_OFILES)
    return tbl + OFF.FDESCENTTBL_HDR
end

local function fd_to_fp(ofiles, fd)
    return kread64(ofiles + fd * OFF.FILEDESCENT_SIZE)
end

local function read_procname(proc, comm_off)
    local buf = malloc(32)
    kread(buf, proc + comm_off, 32)
    local name = ""
    for i = 0, 31 do
        local ch = read8(buf + i)
        if ch == 0 then break end
        if ch < 0x20 or ch > 0x7E then return "" end
        name = name .. string.char(ch)
    end
    return name
end

-- proc offset discovery

local function find_proc_offsets()
    local buf = malloc(0x1000)
    kread(buf, curproc, 0x1000)

    local sysent_off
    for i = 0, 0x1000 - 8 do
        if read64(buf + i) == 0x7FFFFFFFFFFFFFFF then
            sysent_off = i - 0x10
            break
        end
    end
    if not sysent_off or sysent_off < 0 then
        return fail("PROC_SYSENT not found")
    end

    local comm_off
    for i = 0, 0x1000 - 6 do
        if read8(buf + i) == 0xCE and read8(buf + i+1) == 0xFA
        and read8(buf + i+2) == 0xEF and read8(buf + i+3) == 0xBE
        and read8(buf + i+4) == 0xCC and read8(buf + i+5) == 0xBB then
            comm_off = i + 8
            break
        end
    end
    if not comm_off then
        return fail("PROC_COMM not found")
    end

    return sysent_off, comm_off
end

-- sysent swap

local function find_native_process(comm_off)
    local names = { "SceGameLiveStreaming", "SceRedisServer", "SceSysAvControl" }
    local p = kread64(gpu.data_base + OFF.ALLPROC)

    while p ~= 0 and (p >> 48) == 0xFFFF do
        local name = read_procname(p, comm_off)
        for _, target in ipairs(names) do
            if name == target then return p, name end
        end
        p = kread64(p)
    end
    return fail("no native PS5 process found")
end

local function swap_sysent(sysent_off, target_proc)
    local our   = kread64(curproc + sysent_off)
    local their = kread64(target_proc + sysent_off)

    local saved = {
        sv_size  = kread32(our),
        sv_table = kread64(our + 0x8),
        sysent   = our,
    }

    kwrite32(our,       kread32(their))
    kwrite64(our + 0x8, kread64(their + 0x8))
    return saved
end

local function restore_sysent(saved)
    kwrite32(saved.sysent,       saved.sv_size)
    kwrite64(saved.sysent + 0x8, saved.sv_table)
end

-- ELF loader

local function load_elf(path)
    local fd = syscall.open(path, 0, 0)
    if fd < 0 then return nil end

    local size = syscall.lseek(fd, 0, 2)
    syscall.lseek(fd, 0, 0)

    local store = malloc(size)
    syscall.read(fd, store, size)
    syscall.close(fd)

    if read32(store) ~= 0x464C457F then return nil end
    return store
end

local function map_elf(store)
    local e_entry = read64(store + 0x18)
    local e_phoff = read64(store + 0x20)
    local e_shoff = read64(store + 0x28)
    local e_phnum = read16(store + 0x38)
    local e_shnum = read16(store + 0x3C)
    local exec_start, exec_end = 0, 0

    for i = 0, e_phnum - 1 do
        local ph = store + e_phoff + i * 0x38
        local p_type   = read32(ph)
        local p_flags  = read32(ph + 4)
        local p_off    = read64(ph + 0x08)
        local p_vaddr  = read64(ph + 0x10)
        local p_filesz = read64(ph + 0x20)
        local p_memsz  = read64(ph + 0x28)

        if p_type == 1 then
            local aligned = (p_memsz + 0x3FFF) & 0xFFFFC000

            if (p_flags & 1) == 1 then
                exec_start = p_vaddr
                exec_end   = p_vaddr + p_memsz
                local eh = syscall.jitshm_create(0, aligned, PROT_RWX)
                local wh = syscall.jitshm_alias(eh, PROT_RW)
                syscall.mmap(SHADOW_MAPPING_ADDR, aligned, PROT_RW, MAP_SHARED_FIXED, wh, 0)
                if p_filesz > 0 then memcpy(SHADOW_MAPPING_ADDR, store + p_off, p_filesz) end
                if p_memsz > p_filesz then memset(SHADOW_MAPPING_ADDR + p_filesz, 0, p_memsz - p_filesz) end
                syscall.mmap(MAPPING_ADDR + p_vaddr, aligned, PROT_RWX, MAP_SHARED_FIXED, eh, 0)
            else
                syscall.mmap(MAPPING_ADDR + p_vaddr, aligned, PROT_RW, MAP_ANON_PRIVATE_FIXED, 0xFFFFFFFF, 0)
                if p_filesz > 0 then memcpy(MAPPING_ADDR + p_vaddr, store + p_off, p_filesz) end
            end
        end
    end

    local reloc_count = 0
    for i = 0, e_shnum - 1 do
        local sh = store + e_shoff + i * 0x40
        if read32(sh + 4) == 4 then
            local sh_off  = read64(sh + 0x18)
            local sh_size = read64(sh + 0x20)
            for j = 0, (sh_size // 0x18) - 1 do
                local r = store + sh_off + j * 0x18
                local r_offset = read64(r)
                local r_info   = read64(r + 8)
                local r_addend = read64(r + 0x10)

                if (r_info & 0xFF) == 0x08 then
                    local dst = (r_offset >= exec_start and r_offset < exec_end)
                        and (SHADOW_MAPPING_ADDR + r_offset)
                        or  (MAPPING_ADDR + r_offset)
                    write64(dst, MAPPING_ADDR + r_addend)
                    reloc_count = reloc_count + 1
                end
            end
        end
    end

    return MAPPING_ADDR + e_entry, reloc_count
end

-- kernel r/w primitives for the loaded ELF

local function create_pipe()
    local fPipe = func_wrap(dlsym(LIBKERNEL_HANDLE, "pipe"))
    local buf   = malloc(8)

    write32(buf, 0); write32(buf + 4, 0)
    local ret = fPipe(buf)
    local rfd, wfd = read32(buf), read32(buf + 4)
    if ret ~= 0 or rfd <= 0 or wfd <= 0 then
        return fail("pipe() failed")
    end

    local fp    = fd_to_fp(get_ofiles(), rfd)
    local kpipe = kread64(fp)
    if kpipe == 0 then
        return fail("kpipe is NULL")
    end

    return rfd, wfd, kpipe
end

local function create_overlapped_sockets()
    local ms = create_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)
    local vs = create_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)

    local mbuf = malloc(20)
    for i = 0, 4 do write32(mbuf + i * 4, 0) end
    syscall.setsockopt(ms, IPPROTO_IPV6, IPV6_PKTINFO, mbuf, 20)

    local vbuf = malloc(20)
    for i = 0, 4 do write32(vbuf + i * 4, 0) end
    syscall.setsockopt(vs, IPPROTO_IPV6, IPV6_PKTINFO, vbuf, 20)

    local ofiles = get_ofiles()

    local mso  = kread64(fd_to_fp(ofiles, ms))
    local mpcb = kread64(mso + 0x18)
    local mp   = kread64(mpcb + OFF.INPCB_PKTOPTS)

    local vso  = kread64(fd_to_fp(ofiles, vs))
    local vpcb = kread64(vso + 0x18)
    local vp   = kread64(vpcb + OFF.INPCB_PKTOPTS)

    if mp == 0 or vp == 0 then
        return fail("pktopts allocation failed")
    end

    kwrite32(mso, kread32(mso) + 0x100)
    kwrite32(vso, kread32(vso) + 0x100)

    kwrite64(mp + 0x10, vp + 0x10)

    return ms, vs
end


function load_elfldr()
    ulog("====== elf_loader ======")

    syscall.resolve({
        open = 0x5, lseek = 0x1DE, mmap = 0x1DD, munmap = 0x49,
        jitshm_create = 0x215, jitshm_alias = 0x216,
    })

    local fPJ = func_wrap(dlsym(LIBKERNEL_HANDLE, "scePthreadJoin"))

    local PROC_SYSENT, PROC_COMM = find_proc_offsets()
    if not PROC_SYSENT then return end

    local target, tname = find_native_process(PROC_COMM)
    if not target then return end
    ulog("target: " .. tname)

    local saved = swap_sysent(PROC_SYSENT, target)

    local title_id = get_title_id()

    local ok, err = pcall(function()
        local fw = tonumber(FW_VERSION)

        if fw > 13.20 then
            error("Unsupported firmware " .. FW_VERSION)
        end

        local elf
        if fw > 10.01 then
            elf = "elfldr_1320.elf"
        else
            elf = "elfldr_1001.elf"
        end

        local search = {
            "/mnt/sandbox/" .. title_id .. "_000/savedata0/lua/elf_jb/" .. elf,
            "/mnt/sandbox/" .. title_id .. "_001/savedata0/lua/elf_jb/" .. elf,
            "/mnt/sandbox/" .. title_id .. "_002/savedata0/lua/elf_jb/" .. elf,
        }
        
        local store, path
        for _, p in ipairs(search) do
            store = load_elf(p)
            if store then path = p; break end
        end
        if not store then fail("elfldr.elf not found"); return end
        ulog("loaded " .. path)

        local entry, nrelocs = map_elf(store)
        ulog("entry=" .. to_hex(entry) .. " relocs=" .. nrelocs)

        local prfd, pwfd, kpipe = create_pipe()
        if not prfd then return end
        local ofiles = get_ofiles()
        local prfp = fd_to_fp(ofiles, prfd)
        local pwfp = fd_to_fp(ofiles, pwfd)
        kwrite32(prfp + 0x28, kread32(prfp + 0x28) + 0x100)
        kwrite32(pwfp + 0x28, kread32(pwfp + 0x28) + 0x100)

        local ms, vs = create_overlapped_sockets()
        if not ms then return end

        local rwpipe = malloc(8)
        write32(rwpipe, prfd); write32(rwpipe + 4, pwfd)

        local rwpair = malloc(8)
        write32(rwpair, ms); write32(rwpair + 4, vs)

        local payloadout = malloc(4)
        write32(payloadout, 0)

        local args = malloc(0x30)
        write64(args + 0x00, dlsym(LIBKERNEL_HANDLE, "getpid"))
        write64(args + 0x08, rwpipe)
        write64(args + 0x10, rwpair)
        write64(args + 0x18, kpipe)
        write64(args + 0x20, gpu.data_base)
        write64(args + 0x28, payloadout)

        local th = malloc(8); write64(th, 0)
        local at = malloc(0x100)
        fAI(at); fAS(at, 0x80000)
        local ret = fPC(th, at, entry, args, "elfldr")
        fAD(at)
        if ret ~= 0 then
            fail("pthread_create failed: " .. to_hex(ret))
            return
        end

        ulog("running...")
        fPJ(read64(th), 0)

        ulog("payloadout=" .. to_hex(read32(payloadout)))
    end)

    restore_sysent(saved)
    ulog("sysent restored")

    if not ok then
        ulog("[elf] ERROR: " .. tostring(err))
        send_notification("elf_loader: " .. tostring(err))
    end
    ulog("====== done ======")
end