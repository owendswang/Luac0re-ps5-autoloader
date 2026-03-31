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

function elf_sender:sceNetSend(sockfd, buf, len, flags, addr, addrlen)
    return syscall.sendto(sockfd, buf, len, flags, addr, addrlen)
end
function elf_sender:sceNetSocket(domain, type, protocol)
    return create_socket(domain, type, protocol)
end
function elf_sender:sceNetSocketClose(sockfd)
    return syscall.close(sockfd)
end
function elf_sender:htons(port)
    local hi = math.floor(port / 256) % 256
    local lo = port % 256
    return lo * 256 + hi
end

function elf_sender:send_to_localhost(port)
    local sockaddr = malloc(16)
    local enable = malloc(4)

    local sockfd = elf_sender:sceNetSocket(AF_INET, SOCK_STREAM, 0)
    if sock_fd == -1 then
        error("socket failed: " .. to_hex(sock_fd))
    end

    write32(enable, 1)
    syscall.setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, enable, 4)

    write8(sockaddr + 1, AF_INET) -- AF_INET
    write16(sockaddr + 2, elf_sender:htons(port))
    write32(sockaddr + 4, 0x0100007F) -- 127.0.0.1

    local ret = syscall.connect(sockfd, sockaddr, 16)
    if ret < 0 then
        elf_sender:sceNetSocketClose(sockfd)
        send_notification("error connecting to localhost:" .. port)
        return
    end

    local buf = malloc(#self.elf_data)
    write_buffer(buf, self.elf_data)
    local total_sent = elf_sender:sceNetSend(sockfd, buf, #self.elf_data, 0, sockaddr, 16)
    elf_sender:sceNetSocketClose(sockfd)
    if total_sent < 0 then
        send_notification("error sending elf data to localhost")
        return
    end
    ulog(string.format("Successfully sent %d bytes to loader ", total_sent))
end


function main()
    send_notification("Autoloader v0.3\nfrom itsPLK/ps5_lua_autoloader")

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

    microsleep(1000000)
    elf_sender:load_from_file(full_path):send_to_localhost(9021)
    -- send_notification("Loader finished!")
    syscall.kill(syscall.getpid(), 9)
end

main()
