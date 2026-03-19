
-- dlsym_test shellcode
local shellcode = "4883EC184531C94881C7A91A0300BA0120000048C744240800000000488D0D3E0000006A006A004C8D442418E80F000000488B4424184883C428C30F1F4400004889F34889F84889D74889CE4C89C24C89C94C8B4424084C8B4C2410FFD0C30F0B7265616400"

if not sceKernelDlsym then
    init_dlsym()
end

write_shellcode(SHELLCODE_BASE, shellcode)

local dlsym_shellcode_test = func_wrap(SHELLCODE_BASE)
local read_addr = dlsym_shellcode_test(EBOOT_BASE, SCE_KERNEL_DLSYM)

send_notification("read function address in libkernel : ".. to_hex(read_addr))

