LOG_SERVER = "192.168.1.180"

if log_initialized_for ~= LOG_SERVER then
    log_initialized_for = LOG_SERVER

    local function htons(port)
        return ((port << 8) | (port >> 8)) & 0xFFFF
    end
    local function parse_ip_le(ip)
        local a, b, c, d = ip:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")
        return (d<<24) | (c<<16) | (b<<8) | a
    end
    
    if log_sock and log_sock >= 0 then
        syscall.close(log_sock)
        log_sock = -1
    end

    send_notification("Setting log server ip to " .. LOG_SERVER)

    log_sock = create_socket(AF_INET, SOCK_DGRAM, 0)
    if log_sock < 0 then
        error("create_socket failed")
    end

    log_addr = malloc(16)
    write8(log_addr + 1, 2)
    write16(log_addr + 2, htons(8080))
    write32(log_addr + 4, parse_ip_le(LOG_SERVER))

    old_print = old_print or print
    function print(str)
        old_print(str)
        local s = str .. "\n"
        syscall.sendto(log_sock, s, #s, 0, log_addr, 16)
    end
    ulog = print
end
