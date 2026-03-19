#include "types.h"

#define LIBKERNEL_HANDLE 0x2001
#define GADGET_OFFSET    0x31aa9  // call rbx; ret

typedef s32 (*dlsym_t)(s32 handle, const char* sym, void** addr_out);

__attribute__((naked))
static u64 eboot_wrap(void* gadget, void* fn,
                      u64 a1, u64 a2, u64 a3,
                      u64 a4, u64 a5, u64 a6)
{
    __asm__ (
        "mov rbx, rsi\n\t"        // rbx = fn
        "mov rax, rdi\n\t"        // rax = gadget
        "mov rdi, rdx\n\t"        // a1
        "mov rsi, rcx\n\t"        // a2
        "mov rdx, r8\n\t"         // a3
        "mov rcx, r9\n\t"         // a4
        "mov r8,  [rsp + 8]\n\t"  // a5
        "mov r9,  [rsp + 16]\n\t" // a6
        "call rax\n\t"
        "ret"
    );
}

__attribute__((section(".text.start")))
u64 main(u64 EBOOT_BASE, u64 SCE_KERNEL_DLSYM, u64 arg3, u64 arg4, u64 arg5, u64 arg6)
{
    void*   gadget = (void*)(EBOOT_BASE + GADGET_OFFSET);
    dlsym_t dlsym  = (dlsym_t) SCE_KERNEL_DLSYM;

    void* read_addr = 0;
    eboot_wrap(gadget, (void*)dlsym,
               LIBKERNEL_HANDLE, (u64)"read", (u64)&read_addr,
               0, 0, 0);

    return (u64) read_addr;
}