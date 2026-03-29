
-- from itsPLK/ps5_lua_autoloader
-- autoload.lua
-- This script loads and runs Lua scripts or ELF files from a specified directory on the PS5.
-- Lua scripts are executed directly, while ELF files are sent to a local server running on port 9021.

autoload = {}
autoload.options = {
    autoload_dirname = "ps5_autoloader",
    autoload_dirname_alt = "ps5_lua_loader", -- old directory name for backward compatibility
    autoload_config = "autoload.txt",
}


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

    local sockfd = elf_sender:sceNetSocket(2, 1, 0) -- AF_INET=2, SOCK_STREAM=1
    ulog("Socket fd: " .. sockfd)
    assert(sockfd >= 0, "socket creation failed")
    local enable = malloc(4)
    write32(enable, 1)
    syscall.setsockopt(sockfd, 1, 2, enable, 4) -- SOL_SOCKET=1, SO_REUSEADDR=2

    local sockaddr = malloc(16)

    write8(sockaddr + 0, 16)
    write8(sockaddr + 1, 2) -- AF_INET
    write16(sockaddr + 2, elf_sender:htons(port))

    write8(sockaddr + 4, 0x7F) -- 127
    write8(sockaddr + 5, 0x00) -- 0
    write8(sockaddr + 6, 0x00) -- 0
    write8(sockaddr + 7, 0x01) -- 1

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
    send_notification("Autoloader from itsPLK/ps5_lua_autoloader")

    if not is_jailbroken() then
        ulog("Not jailbroken\nSkipped autoloading payloads")
        -- kill_app()
        return
    end

    -- Build possible paths, prioritizing USBs first, then /data, then savedata
    local possible_paths = {}
    for usb = 0, 7 do
        table.insert(possible_paths, string.format("/mnt/usb%d/%s/", usb, autoload.options.autoload_dirname))
        table.insert(possible_paths, string.format("/mnt/usb%d/%s/", usb, autoload.options.autoload_dirname_alt))
    end
    table.insert(possible_paths, string.format("/data/%s/", autoload.options.autoload_dirname))
    table.insert(possible_paths, string.format("/data/%s/", autoload.options.autoload_dirname_alt))
    table.insert(possible_paths, "/mnt/sandbox/" .. get_title_id() .. "_000/savedata0/" .. autoload.options.autoload_dirname .. "/")
    table.insert(possible_paths, "/mnt/sandbox/" .. get_title_id() .. "_000/savedata0/" .. autoload.options.autoload_dirname_alt .. "/")

    local existing_path = nil
    for _, path in ipairs(possible_paths) do
        if file_exists(path .. autoload.options.autoload_config) then
            existing_path = path
            break
        end
    end

    if not existing_path then
        error("autoload config not found")
        return
    end

    send_notification("Loading autoload config from: \n" .. existing_path .. autoload.options.autoload_config)
    local config = io.open(existing_path .. autoload.options.autoload_config, "r")

    for config_line in config:lines() do
        -- trim spaces + \r\n
        config_line = config_line:match("^%s*(.-)%s*$")

        if config_line == "" or config_line:sub(1, 1) == "#" then
            -- skip empty lines and comments
        elseif config_line:sub(1, 1) == "!" then
            -- sleep line
            -- usage: !1000 to sleep for 1000ms
            local sleep_time = tonumber(config_line:sub(2))
            if type(sleep_time) ~= "number" then
                send_notification("[ERROR] Invalid sleep time: \n" .. config_line:sub(2))
                return
            end
            ulog(string.format("Sleeping for: %s ms", sleep_time))
            microsleep(sleep_time * 1000)

        elseif config_line:sub(-4) == ".elf" or config_line:sub(-4) == ".bin" then
            -- error if elfldr is in autoload.txt
            if config_line == "elfldr.elf" or config_line == "elfldr.bin" then
                send_notification("[ERROR] Remove elfldr from autoload.txt")
                return
            end
            local full_path = existing_path .. config_line
            if file_exists(full_path) then
                -- Load the ELF file and send it to localhost on port 9021
                elf_sender:load_from_file(full_path):send_to_localhost(9021)
            else
                send_notification("[ERROR] File not found: \n" .. full_path)
            end

        elseif config_line:sub(-4) == ".lua" then
            -- error if exploit lua script is in autoload.txt
            if config_line == "umtx.lua" or config_line == "lapse.lua" or config_line == "poops_ps5.lua" then
                send_notification("[ERROR] Remove kernel exploit from autoload.txt:\n" .. config_line)
                return
            end
            local full_path = existing_path .. config_line
            if file_exists(full_path) then
                -- Load the Lua script and run it
                send_notification("Loading lua from: \n" .. full_path)
                run_lua_file(full_path)
            else
                send_notification("[ERROR] File not found: \n" .. full_path)
            end

        else
            send_notification("[ERROR] Unsupported file type: \n" .. config_line)
        end

    end
    config:close()

    send_notification("Loader finished!\nClosing game...")
    microsleep(1000)
    kill_app()
end

main()