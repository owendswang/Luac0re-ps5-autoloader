-- autoload.lua
-- Sends the fixed savedata payload ELF to the local ELF loader.


elf_sender = {}
elf_sender.__index = elf_sender

function elf_sender:load_from_file(filepath)
    if file_exists(filepath) then
        send_notification("Loading elf from: \n" .. filepath)
    else
        send_notification("File not found: \n" .. filepath)
    end

    local self = setmetatable({}, elf_sender)
    self.filepath = filepath
    self.elf_data = file_read(filepath)
    self.elf_size = #self.elf_data

    ulog("elf size: " .. self.elf_size)
    return self
end

function elf_sender:htons(port)
    local hi = math.floor(port / 256) % 256
    local lo = port % 256
    return lo * 256 + hi
end

function elf_sender:get_net_ops()
    if PLATFORM == "PS5" and tonumber(FW_VERSION) >= 8.00 and jit_syscall then
        return {
            socket = jit_syscall.socket,
            setsockopt = jit_syscall.setsockopt,
            connect = jit_syscall.connect,
            close = jit_syscall.close,
            error_string = jit_get_error_string,
            is_jit = true
        }
    end

    return {
        socket = syscall.socket,
        setsockopt = syscall.setsockopt,
        connect = syscall.connect,
        close = syscall.close,
        error_string = get_error_string,
        is_jit = false
    }
end

function elf_sender:configure_socket(net, sockfd, optval)
    write32(optval, 1)
    local ret = net.setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, optval, 4)
    if ret < 0 then
        send_notification("setsockopt failed:\n" .. net.error_string())
        return false
    end

    local sndbuf_size = #self.elf_data + 0x10000
    write32(optval, sndbuf_size)
    ret = net.setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, optval, 4)
    if ret < 0 then
        send_notification("setsockopt SO_SNDBUF failed:\n" .. net.error_string())
        return false
    end
    ulog("autoload: SO_SNDBUF=" .. sndbuf_size)

    return true
end

function elf_sender:write_sockaddr(sockaddr, port)
    write64(sockaddr, 0)
    write64(sockaddr + 8, 0)
    write8(sockaddr, 16)         -- sin_len
    write8(sockaddr + 1, AF_INET)
    write16(sockaddr + 2, elf_sender:htons(port))
    write32(sockaddr + 4, 0x0100007F) -- 127.0.0.1
end

function elf_sender:send_all(sockfd)
    local total_sent = 0
    local chunk_size = 0x4000
    local buf = malloc(chunk_size)

    ulog("autoload: sending elf")

    while total_sent < #self.elf_data do
        local remaining = #self.elf_data - total_sent
        local to_send = remaining > chunk_size and chunk_size or remaining
        write_buffer(buf, self.elf_data:sub(total_sent + 1, total_sent + to_send))

        local chunk_sent = 0
        while chunk_sent < to_send do
            local sent = syscall.write(sockfd, buf + chunk_sent, to_send - chunk_sent)
            if sent <= 0 then
                send_notification("send elf data failed:\n" .. get_error_string())
                return false
            end
            chunk_sent = chunk_sent + sent
            total_sent = total_sent + sent
        end
    end

    ulog(string.format("Successfully sent %d bytes to loader ", total_sent))
    return true
end

function elf_sender:send_to_localhost(port)
    local net = elf_sender:get_net_ops()
    local sockaddr = net.is_jit and (OOB_SCRATCH_BASE + 0x7000) or malloc(16)
    local optval = net.is_jit and (OOB_SCRATCH_BASE + 0x7010) or malloc(4)
    local net_name = net.is_jit and "jit_syscall" or "syscall"

    ulog("autoload: net path: " .. net_name)

    local sockfd = net.socket(AF_INET, SOCK_STREAM, 0)
    if sockfd < 0 then
        send_notification("socket failed:\n" .. net.error_string())
        return
    end

    if not self:configure_socket(net, sockfd, optval) then
        net.close(sockfd)
        return
    end

    self:write_sockaddr(sockaddr, port)
    ulog("autoload: connecting to 127.0.0.1:" .. port)
    local ret = net.connect(sockfd, sockaddr, 16)
    if ret < 0 then
        net.close(sockfd)
        send_notification("connect localhost:" .. port .. " failed:\n" .. net.error_string())
        return
    end
    ulog("autoload: connected")

    if net.is_jit then
        ulog("autoload: moving connected fd to main")
        local main_fd = jit_send_recv_fd(sockfd, NEW_JIT_SOCK, NEW_MAIN_SOCK)
        net.close(sockfd)
        if main_fd < 0 then
            send_notification("connected fd transfer failed")
            return
        end

        sockfd = main_fd
        ulog("autoload: using syscall.write")
    end

    self:send_all(sockfd)
    syscall.close(sockfd)
end


function main()
    ulog("Wait for elfldr...")
    microsleep(4000000)

    send_notification("Autoloader v0.6\nbased on itsPLK's work")

    if not is_jailbroken() then
        ulog("Not jailbroken\nSkipped autoloading payloads")
        -- syscall.kill(syscall.getpid(), 9)
        return
    end

    local full_path = "/mnt/sandbox/" .. get_title_id() .. "_000/savedata0/lua/elf_jb/ps5_autoload.elf"
    if not file_exists(full_path) then
        send_notification("[ERROR] File not found: \n" .. full_path)
        return
    end

    elf_sender:load_from_file(full_path):send_to_localhost(9021)
    microsleep(1000000)
    syscall.kill(syscall.getpid(), 9)
end

main()
