/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room cold-path Linux syscall soft stubs (Option C personality).
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0 — no GPL source.
 *
 * Role
 * ----
 * Host / bring-up soft implementations for syscalls that are not on the
 * kernel hot table. Soft depth targets A0 wine-class probes (pipe,
 * eventfd, socketpair, epoll, memfd, dup, poll, write, identity, time).
 * Depth grows with the Deck Top 50 matrix only.
 *
 * Call chain (kernel smoke)
 * -------------------------
 *   linux_dispatch (cold) → cold_ipc → protonrt_service
 *     → (vfs_ram / explicit cases) → protonrt_cold_linux (this file)
 *
 * Ownership / ENOSYS
 * ------------------
 * Unimplemented NRs return -E_NOSYS (-38).
 *
 * io_uring min rings live in the kernel cold path (io_uring_min via
 * protonrt_cold_link): setup / enter / register succeed as min rings
 * on the product path — not an ENOSYS policy for setup. This library
 * never implements those NRs; host-only direct calls still hit the
 * explicit io_uring stubs below (or default) and return ENOSYS.
 * Full SQE execution remains soft until a Deck title hard-requires it.
 *
 * Soft path (1.10)
 * ----------------
 * Typed soft FD table (stdio 0–2 reserved). pipe2 / eventfd2 /
 * socketpair / epoll_create1 / memfd_create allocate kinds; write
 * discards; read EOF (or eventfd counter); poll reports readiness for
 * owned fds; ioctl TTY/FIONREAD soft; fsync/madvise/flock no-op success.
 * No libc: freestanding only.
 *
 * Soft inventory (Wave 104 exclusive deepen; this unit only) — greppable:
 *   "protonrt: soft …"
 *   "cold_linux: soft …"
 * Prefix-stable serial markers:
 *   protonrt: soft inventory|fd|io|fd_alloc|stat|namei|id|time|poll|sock|
 *             proc|uring|enosys|query|path|groups|last|open|deepen|wave …
 *   cold_linux: soft inventory|fd|io|fd_alloc|stat|namei|id|time|poll|sock|
 *               proc|uring|enosys|query|path|groups|last|open|deepen|wave …
 * Pure observation; never hard-gates; wrap OK; soft ≠ bar3.
 * Honesty: soft cold personality ≠ product multi-process UDX/notify close.
 */
#include <stdint.h>
#include <gj/klog.h>
#include "../include/protonrt/protonrt.h"

/* Linux x86_64 syscall numbers used on the cold path. */
#define NR_read          0
#define NR_write         1
#define NR_open          2
#define NR_close         3
#define NR_stat          4
#define NR_fstat         5
#define NR_lstat         6
#define NR_poll          7
#define NR_lseek         8
#define NR_mprotect     10
#define NR_munmap       11
#define NR_brk          12
#define NR_rt_sigaction 13
#define NR_rt_sigprocmask 14
#define NR_ioctl        16
#define NR_pread64      17
#define NR_pwrite64     18
#define NR_readv        19
#define NR_writev       20
#define NR_access       21
#define NR_pipe         22
#define NR_select       23
#define NR_sched_yield  24
#define NR_madvise      28
#define NR_dup          32
#define NR_dup2         33
#define NR_nanosleep    35
#define NR_getpid       39
#define NR_socket       41
#define NR_connect      42
#define NR_accept       43
#define NR_sendto       44
#define NR_recvfrom     45
#define NR_sendmsg      46
#define NR_recvmsg      47
#define NR_shutdown     48
#define NR_bind         49
#define NR_listen       50
#define NR_getsockname  51
#define NR_getpeername  52
#define NR_socketpair   53
#define NR_setsockopt   54
#define NR_getsockopt   55
#define NR_clone        56
#define NR_fork         57
#define NR_execve       59
#define NR_exit         60
#define NR_wait4        61
#define NR_kill         62
#define NR_uname        63
#define NR_fcntl        72
#define NR_flock        73
#define NR_fsync        74
#define NR_fdatasync    75
#define NR_ftruncate    77
#define NR_getdents     78
#define NR_getcwd       79
#define NR_chdir        80
#define NR_fchdir       81
#define NR_rename       82
#define NR_mkdir        83
#define NR_rmdir        84
#define NR_creat        85
#define NR_link         86
#define NR_unlink       87
#define NR_symlink      88
#define NR_readlink     89
#define NR_chmod        90
#define NR_getrusage    98
#define NR_sysinfo      99
#define NR_getuid      102
#define NR_getgid      104
#define NR_geteuid     107
#define NR_getegid     108
#define NR_setpgid     109
#define NR_getppid     110
#define NR_getpgrp     111
#define NR_setsid      112
#define NR_getsid      124
#define NR_personality 135
#define NR_sched_setparam 142
#define NR_sched_getparam 143
#define NR_sched_setscheduler 144
#define NR_sched_getscheduler 145
#define NR_sched_get_priority_max 146
#define NR_sched_get_priority_min 147
#define NR_prctl       157
#define NR_arch_prctl  158
#define NR_sync        162
#define NR_gettid      186
#define NR_getdents64  217
#define NR_set_tid_address 218
#define NR_clock_gettime 228
#define NR_clock_getres  229
#define NR_clock_nanosleep 230
#define NR_epoll_wait  232
#define NR_epoll_ctl   233
#define NR_utimensat   280
#define NR_epoll_pwait 281
#define NR_signalfd4   282
#define NR_timerfd_create 283
#define NR_eventfd     284
#define NR_timerfd_settime 286
#define NR_timerfd_gettime 287
#define NR_eventfd2    290
#define NR_epoll_create1 291
#define NR_dup3        292
#define NR_pipe2       293
#define NR_prlimit64   302
#define NR_syncfs      306
#define NR_getrandom   318
#define NR_memfd_create 319
#define NR_statx       332
#define NR_io_uring_setup    425
#define NR_io_uring_enter    426
#define NR_io_uring_register 427
#define NR_clone3      435
#define NR_close_range 436
#define NR_openat      257
#define NR_mkdirat     258
#define NR_newfstatat  262
#define NR_unlinkat    263
#define NR_renameat    264
#define NR_linkat      265
#define NR_symlinkat   266
#define NR_readlinkat  267
#define NR_faccessat   269
#define NR_pselect6    270
#define NR_ppoll       271
#define NR_epoll_create 213

/* Linux errno values used as negative return codes. */
#define E_PERM     1
#define E_NOENT    2
#define E_SRCH     3
#define E_IO       5
#define E_BADF     9
#define E_AGAIN   11
#define E_NOMEM   12
#define E_ACCES   13
#define E_FAULT   14
#define E_BUSY    16
#define E_EXIST   17
#define E_NODEV   19
#define E_NOTDIR  20
#define E_INVAL   22
#define E_NFILE   23
#define E_MFILE   24
#define E_NOSPC   28
#define E_ROFS    30
#define E_PIPE    32
#define E_NOSYS   38
#define E_NOTSOCK 88

/* Soft FD kinds (stdio 0–2 reserved as KIND_STDIO). */
#define PR_KIND_FREE    0u
#define PR_KIND_STDIO   1u
#define PR_KIND_OPEN    2u
#define PR_KIND_PIPE_R  3u
#define PR_KIND_PIPE_W  4u
#define PR_KIND_EVENTFD 5u
#define PR_KIND_EPOLL   6u
#define PR_KIND_SOCKET  7u
#define PR_KIND_MEMFD   8u
#define PR_KIND_TIMERFD 9u
#define PR_KIND_SIGNALFD 10u

/* fcntl cmds (subset). */
#define PR_F_DUPFD     0
#define PR_F_GETFD     1
#define PR_F_SETFD     2
#define PR_F_GETFL     3
#define PR_F_SETFL     4
#define PR_F_DUPFD_CLOEXEC 1030

/* poll / epoll bit shapes (Linux). */
#define PR_POLLIN   0x0001u
#define PR_POLLOUT  0x0004u
#define PR_POLLERR  0x0008u
#define PR_POLLHUP  0x0010u
#define PR_EPOLL_CTL_ADD 1
#define PR_EPOLL_CTL_DEL 2
#define PR_EPOLL_CTL_MOD 3

/* Soft FD table (stdio 0–2 reserved). */
#define PR_MAX_FD 32
#define PR_EPOLL_SLOTS 8

struct pr_fd {
    uint8_t  u8Kind;
    uint8_t  u8Flags;     /* O_* low bits soft */
    uint8_t  u8Cloexec;
    uint8_t  u8Pad;
    uint32_t u32Peer;     /* peer fd for pipe/socketpair (or self) */
    uint64_t u64Counter;  /* eventfd counter / timer remaining soft */
    uint32_t u32EpollMask[PR_EPOLL_SLOTS]; /* watched fd events (epfd only) */
    int32_t  aEpollFd[PR_EPOLL_SLOTS];
};

static struct pr_fd g_aFd[PR_MAX_FD];
static uint32_t g_u32Prng = 0xC0FFEEu;
static uint64_t g_u64MonoNs = 1000000000ull; /* synthetic mono clock */
static int g_fFdInit;
static char g_szCwd[8] = { '/', 0, 0, 0, 0, 0, 0, 0 };
static uint64_t g_u64ClearChildTid;

/*
 * Soft product inventory (Wave 104 exclusive deepen). Enter-only tallies.
 * greppable: protonrt: soft … / cold_linux: soft …
 * Never rewrites syscall returns; diagnostics / smoke only.
 */
enum {
    COLD_SOFT_GRP_IO = 0,     /* read/write/close/lseek/ioctl/fcntl/fsync/map */
    COLD_SOFT_GRP_FD_ALLOC,   /* open/pipe/eventfd/timerfd/signalfd/epoll_create/dup/memfd */
    COLD_SOFT_GRP_STAT,       /* stat/access/getdents */
    COLD_SOFT_GRP_PATH,       /* getcwd/chdir/mkdir/unlink/rename/link/symlink/readlink */
    COLD_SOFT_GRP_ID,         /* uname/pid/uid/personality/prctl/sched */
    COLD_SOFT_GRP_TIME,       /* clock/nanosleep/sysinfo/rusage/getrandom/prlimit */
    COLD_SOFT_GRP_POLL,       /* poll/select/epoll_ctl/wait */
    COLD_SOFT_GRP_SOCK,       /* socket family */
    COLD_SOFT_GRP_PROC,       /* clone/fork/exec/wait/kill/exit/sig */
    COLD_SOFT_GRP_URING,      /* io_uring_* host ENOSYS stubs */
    COLD_SOFT_GRP_OTHER,      /* unlisted NR → ENOSYS default */
    COLD_SOFT_GRP_N
};

#define COLD_SOFT_WAVE 70u
#define COLD_SOFT_AREAS    20u /* twin inventory + honesty per prefix family */
#define COLD_SOFT_VER_MAJ  ((uint32_t)PROTON_RT_VERSION_MAJOR)
#define COLD_SOFT_VER_MIN  ((uint32_t)PROTON_RT_VERSION_MINOR)

static uint64_t g_aSoftEnter[COLD_SOFT_GRP_N]; /* per-group handler entries */
static uint64_t g_u64SoftEnterTotal;           /* sum of all group enters */
static uint64_t g_u64SoftEnosys;               /* cold_enosys / default hits */
static uint64_t g_u64SoftQueryEnter;           /* proton_rt_query entries */
static uint64_t g_u64SoftQueryOk;              /* query success */
static uint64_t g_u64SoftQueryNull;            /* query pOut == NULL */
static uint64_t g_u64SoftLogN;                 /* inventory log emissions */
static uint64_t g_u64SoftLastNr;               /* last NR classified (soft) */
static uint32_t g_u32SoftLastGrp;              /* last group for last_nr */
static uint8_t  g_fSoftInvOnce;                /* one-shot deep dump after activity */

static void soft_inc(uint64_t *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static uint32_t soft_classify_nr(uint64_t u64Nr);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
soft_inc(uint64_t *pCtr)
{
    if (pCtr == 0) {
        return;
    }
    (*pCtr)++;
}

/**
 * Map Linux x86_64 NR to soft inventory group (enter classification only).
 * greppable: protonrt: soft / cold_linux: soft
 */
static uint32_t
soft_classify_nr(uint64_t u64Nr)
{
    switch (u64Nr) {
    case NR_read:
    case NR_write:
    case NR_close:
    case NR_close_range:
    case NR_lseek:
    case NR_ftruncate:
    case NR_ioctl:
    case NR_fcntl:
    case NR_flock:
    case NR_fsync:
    case NR_fdatasync:
    case NR_sync:
    case NR_syncfs:
    case NR_madvise:
    case NR_mprotect:
    case NR_munmap:
    case NR_brk:
    case NR_pread64:
    case NR_pwrite64:
    case NR_readv:
    case NR_writev:
        return (uint32_t)COLD_SOFT_GRP_IO;

    case NR_open:
    case NR_openat:
    case NR_creat:
    case NR_memfd_create:
    case NR_pipe:
    case NR_pipe2:
    case NR_eventfd:
    case NR_eventfd2:
    case NR_timerfd_create:
    case NR_timerfd_settime:
    case NR_timerfd_gettime:
    case NR_signalfd4:
    case NR_epoll_create:
    case NR_epoll_create1:
    case NR_dup:
    case NR_dup2:
    case NR_dup3:
        return (uint32_t)COLD_SOFT_GRP_FD_ALLOC;

    case NR_fstat:
    case NR_stat:
    case NR_lstat:
    case NR_newfstatat:
    case NR_statx:
    case NR_access:
    case NR_faccessat:
    case NR_getdents:
    case NR_getdents64:
        return (uint32_t)COLD_SOFT_GRP_STAT;

    case NR_getcwd:
    case NR_chdir:
    case NR_fchdir:
    case NR_mkdir:
    case NR_mkdirat:
    case NR_rmdir:
    case NR_unlink:
    case NR_unlinkat:
    case NR_rename:
    case NR_renameat:
    case NR_link:
    case NR_linkat:
    case NR_symlink:
    case NR_symlinkat:
    case NR_chmod:
    case NR_utimensat:
    case NR_readlink:
    case NR_readlinkat:
        return (uint32_t)COLD_SOFT_GRP_PATH;

    case NR_uname:
    case NR_getpid:
    case NR_gettid:
    case NR_getppid:
    case NR_getuid:
    case NR_geteuid:
    case NR_getgid:
    case NR_getegid:
    case NR_getpgrp:
    case NR_getsid:
    case NR_setpgid:
    case NR_setsid:
    case NR_set_tid_address:
    case NR_personality:
    case NR_prctl:
    case NR_arch_prctl:
    case NR_sched_yield:
    case NR_sched_getscheduler:
    case NR_sched_getparam:
    case NR_sched_setscheduler:
    case NR_sched_setparam:
    case NR_sched_get_priority_max:
    case NR_sched_get_priority_min:
        return (uint32_t)COLD_SOFT_GRP_ID;

    case NR_getrandom:
    case NR_prlimit64:
    case NR_clock_gettime:
    case NR_clock_getres:
    case NR_clock_nanosleep:
    case NR_nanosleep:
    case NR_sysinfo:
    case NR_getrusage:
        return (uint32_t)COLD_SOFT_GRP_TIME;

    case NR_poll:
    case NR_ppoll:
    case NR_select:
    case NR_pselect6:
    case NR_epoll_ctl:
    case NR_epoll_wait:
    case NR_epoll_pwait:
        return (uint32_t)COLD_SOFT_GRP_POLL;

    case NR_socket:
    case NR_socketpair:
    case NR_bind:
    case NR_listen:
    case NR_connect:
    case NR_shutdown:
    case NR_setsockopt:
    case NR_getsockopt:
    case NR_getsockname:
    case NR_getpeername:
    case NR_accept:
    case NR_sendto:
    case NR_sendmsg:
    case NR_recvfrom:
    case NR_recvmsg:
        return (uint32_t)COLD_SOFT_GRP_SOCK;

    case NR_clone:
    case NR_clone3:
    case NR_fork:
    case NR_execve:
    case NR_wait4:
    case NR_kill:
    case NR_exit:
    case NR_rt_sigaction:
    case NR_rt_sigprocmask:
        return (uint32_t)COLD_SOFT_GRP_PROC;

    case NR_io_uring_setup:
    case NR_io_uring_enter:
    case NR_io_uring_register:
        return (uint32_t)COLD_SOFT_GRP_URING;

    default:
        return (uint32_t)COLD_SOFT_GRP_OTHER;
    }
}

/**
 * Count live soft FD table kinds (stdio reserved + allocated).
 * Diagnostics only — never mutates table.
 */
static void
soft_fd_live_counts(uint32_t *pFree, uint32_t *pLive, uint32_t *pStdio,
                    uint32_t *pPipe, uint32_t *pEvent, uint32_t *pEpoll,
                    uint32_t *pSock, uint32_t *pMemfd, uint32_t *pTimer,
                    uint32_t *pOpen, uint32_t *pSig)
{
    uint32_t i;
    uint32_t u32Free = 0;
    uint32_t u32Live = 0;
    uint32_t u32Stdio = 0;
    uint32_t u32Pipe = 0;
    uint32_t u32Event = 0;
    uint32_t u32Epoll = 0;
    uint32_t u32Sock = 0;
    uint32_t u32Memfd = 0;
    uint32_t u32Timer = 0;
    uint32_t u32Open = 0;
    uint32_t u32Sig = 0;

    for (i = 0; i < (uint32_t)PR_MAX_FD; i++) {
        uint8_t u8Kind = g_aFd[i].u8Kind;

        switch (u8Kind) {
        case PR_KIND_FREE:
            u32Free++;
            break;
        case PR_KIND_STDIO:
            u32Stdio++;
            u32Live++;
            break;
        case PR_KIND_PIPE_R:
        case PR_KIND_PIPE_W:
            u32Pipe++;
            u32Live++;
            break;
        case PR_KIND_EVENTFD:
            u32Event++;
            u32Live++;
            break;
        case PR_KIND_EPOLL:
            u32Epoll++;
            u32Live++;
            break;
        case PR_KIND_SOCKET:
            u32Sock++;
            u32Live++;
            break;
        case PR_KIND_MEMFD:
            u32Memfd++;
            u32Live++;
            break;
        case PR_KIND_TIMERFD:
            u32Timer++;
            u32Live++;
            break;
        case PR_KIND_OPEN:
            u32Open++;
            u32Live++;
            break;
        case PR_KIND_SIGNALFD:
            u32Sig++;
            u32Live++;
            break;
        default:
            u32Live++;
            break;
        }
    }
    if (pFree != 0) {
        *pFree = u32Free;
    }
    if (pLive != 0) {
        *pLive = u32Live;
    }
    if (pStdio != 0) {
        *pStdio = u32Stdio;
    }
    if (pPipe != 0) {
        *pPipe = u32Pipe;
    }
    if (pEvent != 0) {
        *pEvent = u32Event;
    }
    if (pEpoll != 0) {
        *pEpoll = u32Epoll;
    }
    if (pSock != 0) {
        *pSock = u32Sock;
    }
    if (pMemfd != 0) {
        *pMemfd = u32Memfd;
    }
    if (pTimer != 0) {
        *pTimer = u32Timer;
    }
    if (pOpen != 0) {
        *pOpen = u32Open;
    }
    if (pSig != 0) {
        *pSig = u32Sig;
    }
}

/**
 * Greppable soft protonrt / cold_linux inventory (product / smoke).
 * Twin prefixes so either agent grep works (Wave 104 exclusive deepen):
 *   protonrt: soft inventory|fd|io|fd_alloc|stat|namei|id|time|poll|sock|
 *             proc|uring|enosys|query|path|groups|last|open|deepen|wave …
 *   cold_linux: soft inventory|fd|io|fd_alloc|stat|namei|id|time|poll|sock|
 *               proc|uring|enosys|query|path|groups|last|open|deepen|wave …
 * greppable: protonrt: soft
 * greppable: cold_linux: soft
 */
static void
soft_inventory_log(void)
{
    uint32_t u32Free;
    uint32_t u32Live;
    uint32_t u32Stdio;
    uint32_t u32Pipe;
    uint32_t u32Event;
    uint32_t u32Epoll;
    uint32_t u32Sock;
    uint32_t u32Memfd;
    uint32_t u32Timer;
    uint32_t u32Open;
    uint32_t u32Sig;
    uint32_t u32FdInit;
    uint32_t u32Feat;

    soft_inc(&g_u64SoftLogN);
    soft_fd_live_counts(&u32Free, &u32Live, &u32Stdio, &u32Pipe, &u32Event,
                        &u32Epoll, &u32Sock, &u32Memfd, &u32Timer, &u32Open,
                        &u32Sig);
    u32FdInit = g_fFdInit ? 1u : 0u;
    u32Feat = PROTON_FEAT_LINUX_COMPAT | PROTON_FEAT_FUTEX |
              PROTON_FEAT_NAMED_SHM | PROTON_FEAT_SOCKETPAIR |
              PROTON_FEAT_EVENTFD | PROTON_FEAT_EPOLL | PROTON_FEAT_PIPE |
              PROTON_FEAT_MEMFD;

    /*
     * Primary prefix: protonrt: soft …
     * Catalog capacity + live FD table + group enter tallies.
     */
    /* Grep: protonrt: soft inventory */
    kprintf("protonrt: soft inventory wave=%u ver=%u.%u max_fd=%u "
            "epoll_slots=%u groups=%u enter=%llu enosys=%llu query=%llu "
            "log_n=%llu fd_init=%u features=0x%x\n",
            (unsigned)COLD_SOFT_WAVE,
            (unsigned)COLD_SOFT_VER_MAJ, (unsigned)COLD_SOFT_VER_MIN,
            (unsigned)PR_MAX_FD, (unsigned)PR_EPOLL_SLOTS,
            (unsigned)COLD_SOFT_GRP_N,
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_u64SoftQueryEnter,
            (unsigned long long)g_u64SoftLogN, u32FdInit, u32Feat);

    /* Grep: protonrt: soft fd */
    kprintf("protonrt: soft fd free=%u live=%u stdio=%u pipe=%u event=%u "
            "epoll=%u sock=%u memfd=%u timer=%u open=%u sig=%u max=%u\n",
            u32Free, u32Live, u32Stdio, u32Pipe, u32Event, u32Epoll, u32Sock,
            u32Memfd, u32Timer, u32Open, u32Sig, (unsigned)PR_MAX_FD);

    /* Grep: protonrt: soft io */
    kprintf("protonrt: soft io enter=%llu "
            "surface=read,write,close,lseek,ioctl,fcntl,fsync,madvise,brk,"
            "readv,writev\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_IO]);

    /* Grep: protonrt: soft fd_alloc */
    kprintf("protonrt: soft fd_alloc enter=%llu "
            "surface=open,pipe,eventfd,timerfd,signalfd,epoll_create,dup,"
            "memfd\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_FD_ALLOC]);

    /* Grep: protonrt: soft stat */
    kprintf("protonrt: soft stat enter=%llu "
            "surface=fstat,stat,lstat,newfstatat,statx,access,getdents\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_STAT]);

    /* Grep: protonrt: soft namei */
    kprintf("protonrt: soft namei enter=%llu "
            "surface=getcwd,chdir,mkdir,unlink,rename,link,symlink,"
            "readlink,chmod,utimensat\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PATH]);

    /* Grep: protonrt: soft id */
    kprintf("protonrt: soft id enter=%llu "
            "surface=uname,getpid,uid,gid,sid,personality,prctl,sched\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_ID]);

    /* Grep: protonrt: soft time */
    kprintf("protonrt: soft time enter=%llu "
            "surface=clock_*,nanosleep,sysinfo,rusage,getrandom,prlimit\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_TIME]);

    /* Grep: protonrt: soft poll */
    kprintf("protonrt: soft poll enter=%llu "
            "surface=poll,ppoll,select,pselect6,epoll_ctl,epoll_wait\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_POLL]);

    /* Grep: protonrt: soft sock */
    kprintf("protonrt: soft sock enter=%llu "
            "surface=socket,socketpair,bind,listen,connect,send,recv,"
            "getsockopt\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_SOCK]);

    /* Grep: protonrt: soft proc */
    kprintf("protonrt: soft proc enter=%llu "
            "surface=clone,fork,execve,wait4,kill,exit,rt_sig* "
            "host_enosys=spawn_native\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PROC]);

    /* Grep: protonrt: soft uring */
    kprintf("protonrt: soft uring enter=%llu "
            "surface=io_uring_setup,enter,register host=ENOSYS "
            "product=kernel_min_rings\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_URING]);

    /* Grep: protonrt: soft enosys */
    kprintf("protonrt: soft enosys hits=%llu other_enter=%llu "
            "errno=%u claim=unimplemented_nr_or_spawn\n",
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_OTHER],
            (unsigned)E_NOSYS);

    /* Grep: protonrt: soft query */
    kprintf("protonrt: soft query enter=%llu ok=%llu null=%llu "
            "ver=%u.%u features=0x%x\n",
            (unsigned long long)g_u64SoftQueryEnter,
            (unsigned long long)g_u64SoftQueryOk,
            (unsigned long long)g_u64SoftQueryNull,
            (unsigned)COLD_SOFT_VER_MAJ, (unsigned)COLD_SOFT_VER_MIN, u32Feat);

    /* Grep: protonrt: soft path */
    kprintf("protonrt: soft path claim=cold_linux_personality hybrid=OptionC "
            "hot=kernel cold=libprotonrt enter_only=1 ret_rewrite=0 "
            "(soft inventory; not bar3)\n");

    /* Grep: protonrt: soft groups (Wave 104 deepen — all enters one line) */
/* Wave 104 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retlinearangle continuum_toward=24600 soft_ne_product=1 wave=104
 *   greppable: soft retbilinearangle exclusive=1 continuum_toward=24600 soft_ne_product=1 wave=104
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

    kprintf("protonrt: soft groups io=%llu fd_alloc=%llu stat=%llu "
            "namei=%llu id=%llu time=%llu poll=%llu sock=%llu proc=%llu "
            "uring=%llu other=%llu total=%llu wave=%u\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_IO],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_FD_ALLOC],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_STAT],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PATH],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_ID],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_TIME],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_POLL],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_SOCK],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PROC],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_URING],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_OTHER],
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned)COLD_SOFT_WAVE);

    /* Grep: protonrt: soft last (Wave 104 deepen) */
    kprintf("protonrt: soft last nr=%llu grp=%u enter_total=%llu "
            "enosys=%llu log_n=%llu wave=%u\n",
            (unsigned long long)g_u64SoftLastNr,
            (unsigned)g_u32SoftLastGrp,
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_u64SoftLogN,
            (unsigned)COLD_SOFT_WAVE);

    /*
     * Product OPEN honesty — cold soft ≠ multi-process UDX/notify product.
     * greppable: protonrt: soft open
     */
    kprintf("protonrt: soft open kernel_notify=OPEN multi_process_spsc=OPEN "
            "driver_host=OPEN bar3=OPEN product=0 soft=1 wave=%u\n",
            (unsigned)COLD_SOFT_WAVE);

    /* Grep: protonrt: soft deepen wave (Wave 104 stamp) */
    kprintf("protonrt: soft deepen wave=%u areas=%u unit=cold_linux "
            "exclusive=1 prefix=protonrt:_soft log_n=%llu "
            "(soft inventory; not bar3)\n",
            (unsigned)COLD_SOFT_WAVE, (unsigned)COLD_SOFT_AREAS,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: protonrt: soft wave */
    kprintf("protonrt: soft wave n=%u unit=cold_linux exclusive=1 "
            "areas=%u kernel_notify=OPEN multi_process_spsc=OPEN "
            "(soft inventory; not bar3)\n",
            (unsigned)COLD_SOFT_WAVE, (unsigned)COLD_SOFT_AREAS);

    /*
     * Grep: protonrt: soft honesty (Wave 104 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    kprintf("protonrt: soft honesty multi_server=0 confine=0 bar3=0 "
            "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
            (unsigned)COLD_SOFT_WAVE);

    /*
     * Twin prefix: cold_linux: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: cold_linux: soft inventory */
    kprintf("cold_linux: soft inventory wave=%u ver=%u.%u max_fd=%u "
            "epoll_slots=%u groups=%u enter=%llu enosys=%llu query=%llu "
            "log_n=%llu fd_init=%u features=0x%x\n",
            (unsigned)COLD_SOFT_WAVE,
            (unsigned)COLD_SOFT_VER_MAJ, (unsigned)COLD_SOFT_VER_MIN,
            (unsigned)PR_MAX_FD, (unsigned)PR_EPOLL_SLOTS,
            (unsigned)COLD_SOFT_GRP_N,
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_u64SoftQueryEnter,
            (unsigned long long)g_u64SoftLogN, u32FdInit, u32Feat);

    /* Grep: cold_linux: soft fd */
    kprintf("cold_linux: soft fd free=%u live=%u stdio=%u pipe=%u event=%u "
            "epoll=%u sock=%u memfd=%u timer=%u open=%u sig=%u max=%u\n",
            u32Free, u32Live, u32Stdio, u32Pipe, u32Event, u32Epoll, u32Sock,
            u32Memfd, u32Timer, u32Open, u32Sig, (unsigned)PR_MAX_FD);

    /* Grep: cold_linux: soft io */
    kprintf("cold_linux: soft io enter=%llu "
            "surface=read,write,close,lseek,ioctl,fcntl,fsync,madvise,brk,"
            "readv,writev\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_IO]);

    /* Grep: cold_linux: soft fd_alloc */
    kprintf("cold_linux: soft fd_alloc enter=%llu "
            "surface=open,pipe,eventfd,timerfd,signalfd,epoll_create,dup,"
            "memfd\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_FD_ALLOC]);

    /* Grep: cold_linux: soft stat */
    kprintf("cold_linux: soft stat enter=%llu "
            "surface=fstat,stat,lstat,newfstatat,statx,access,getdents\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_STAT]);

    /* Grep: cold_linux: soft namei */
    kprintf("cold_linux: soft namei enter=%llu "
            "surface=getcwd,chdir,mkdir,unlink,rename,link,symlink,"
            "readlink,chmod,utimensat\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PATH]);

    /* Grep: cold_linux: soft id */
    kprintf("cold_linux: soft id enter=%llu "
            "surface=uname,getpid,uid,gid,sid,personality,prctl,sched\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_ID]);

    /* Grep: cold_linux: soft time */
    kprintf("cold_linux: soft time enter=%llu "
            "surface=clock_*,nanosleep,sysinfo,rusage,getrandom,prlimit\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_TIME]);

    /* Grep: cold_linux: soft poll */
    kprintf("cold_linux: soft poll enter=%llu "
            "surface=poll,ppoll,select,pselect6,epoll_ctl,epoll_wait\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_POLL]);

    /* Grep: cold_linux: soft sock */
    kprintf("cold_linux: soft sock enter=%llu "
            "surface=socket,socketpair,bind,listen,connect,send,recv,"
            "getsockopt\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_SOCK]);

    /* Grep: cold_linux: soft proc */
    kprintf("cold_linux: soft proc enter=%llu "
            "surface=clone,fork,execve,wait4,kill,exit,rt_sig* "
            "host_enosys=spawn_native\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PROC]);

    /* Grep: cold_linux: soft uring */
    kprintf("cold_linux: soft uring enter=%llu "
            "surface=io_uring_setup,enter,register host=ENOSYS "
            "product=kernel_min_rings\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_URING]);

    /* Grep: cold_linux: soft enosys */
    kprintf("cold_linux: soft enosys hits=%llu other_enter=%llu "
            "errno=%u claim=unimplemented_nr_or_spawn\n",
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_OTHER],
            (unsigned)E_NOSYS);

    /* Grep: cold_linux: soft query */
    kprintf("cold_linux: soft query enter=%llu ok=%llu null=%llu "
            "ver=%u.%u features=0x%x\n",
            (unsigned long long)g_u64SoftQueryEnter,
            (unsigned long long)g_u64SoftQueryOk,
            (unsigned long long)g_u64SoftQueryNull,
            (unsigned)COLD_SOFT_VER_MAJ, (unsigned)COLD_SOFT_VER_MIN, u32Feat);

    /* Grep: cold_linux: soft path */
    kprintf("cold_linux: soft path claim=cold_linux_personality "
            "hybrid=OptionC hot=kernel cold=libprotonrt enter_only=1 "
            "ret_rewrite=0 (soft inventory; not bar3)\n");

    /* Grep: cold_linux: soft groups (Wave 104 deepen) */
    kprintf("cold_linux: soft groups io=%llu fd_alloc=%llu stat=%llu "
            "namei=%llu id=%llu time=%llu poll=%llu sock=%llu proc=%llu "
            "uring=%llu other=%llu total=%llu wave=%u\n",
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_IO],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_FD_ALLOC],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_STAT],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PATH],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_ID],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_TIME],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_POLL],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_SOCK],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_PROC],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_URING],
            (unsigned long long)g_aSoftEnter[COLD_SOFT_GRP_OTHER],
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned)COLD_SOFT_WAVE);

    /* Grep: cold_linux: soft last (Wave 104 deepen) */
    kprintf("cold_linux: soft last nr=%llu grp=%u enter_total=%llu "
            "enosys=%llu log_n=%llu wave=%u\n",
            (unsigned long long)g_u64SoftLastNr,
            (unsigned)g_u32SoftLastGrp,
            (unsigned long long)g_u64SoftEnterTotal,
            (unsigned long long)g_u64SoftEnosys,
            (unsigned long long)g_u64SoftLogN,
            (unsigned)COLD_SOFT_WAVE);

    /* Grep: cold_linux: soft open (Wave 104 honesty) */
    kprintf("cold_linux: soft open kernel_notify=OPEN multi_process_spsc=OPEN "
            "driver_host=OPEN bar3=OPEN product=0 soft=1 wave=%u\n",
            (unsigned)COLD_SOFT_WAVE);

    /* Grep: cold_linux: soft deepen wave (Wave 104 stamp) */
    kprintf("cold_linux: soft deepen wave=%u areas=%u unit=cold_linux "
            "exclusive=1 prefix=cold_linux:_soft log_n=%llu "
            "(soft inventory; not bar3)\n",
            (unsigned)COLD_SOFT_WAVE, (unsigned)COLD_SOFT_AREAS,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: cold_linux: soft wave */
    kprintf("cold_linux: soft wave n=%u unit=cold_linux exclusive=1 "
            "areas=%u kernel_notify=OPEN multi_process_spsc=OPEN "
            "(soft inventory; not bar3)\n",
            (unsigned)COLD_SOFT_WAVE, (unsigned)COLD_SOFT_AREAS);

    /* Grep: cold_linux: soft honesty (Wave 104 exclusive deepen) */
    kprintf("cold_linux: soft honesty multi_server=0 confine=0 bar3=0 "
            "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
            (unsigned)COLD_SOFT_WAVE);
}

/**
 * After first product query/cold enter, print soft inventory once
 * (mirrors cold_ipc/wow64 soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u64SoftEnterTotal == 0 && g_u64SoftQueryEnter == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

/*
 * Canonical cold-path "not implemented here" result.
 * Prefer this over scattering bare -E_NOSYS so host and kernel-smoke
 * fall-throughs stay one contract.
 */
static int64_t
cold_enosys(void)
{
    soft_inc(&g_u64SoftEnosys);
    return -(int64_t)E_NOSYS;
}

static void
soft_zero(void *pDst, uint64_t cb)
{
    uint8_t *p = (uint8_t *)pDst;
    uint64_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static void
soft_copy(void *pDst, const void *pSrc, uint64_t cb)
{
    uint8_t *pD = (uint8_t *)pDst;
    const uint8_t *pS = (const uint8_t *)pSrc;
    uint64_t i;

    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
}

static void
fd_table_init(void)
{
    int nFd;

    if (g_fFdInit) {
        return;
    }
    soft_zero(g_aFd, sizeof(g_aFd));
    for (nFd = 0; nFd < 3; nFd++) {
        g_aFd[nFd].u8Kind = PR_KIND_STDIO;
        g_aFd[nFd].u32Peer = (uint32_t)nFd;
    }
    g_fFdInit = 1;
}

int
proton_rt_query(struct proton_rt_info *pOut)
{
    soft_inc(&g_u64SoftQueryEnter);
    if (pOut == 0) {
        soft_inc(&g_u64SoftQueryNull);
        soft_inventory_maybe_once();
        return -1;
    }
    /* Packed version: (major << 16) | minor — keep in sync with header. */
    pOut->u32Version =
        ((uint32_t)PROTON_RT_VERSION_MAJOR << 16) | (uint32_t)PROTON_RT_VERSION_MINOR;
    /*
     * Feature mask (libprotonrt 1.10). Hot-path bodies live in the kernel;
     * these bits advertise the personality surface the matrix expects.
     * Soft path now covers pipe/eventfd/socketpair/epoll/memfd kinds.
     */
    pOut->u32Features = PROTON_FEAT_LINUX_COMPAT | PROTON_FEAT_FUTEX |
                        PROTON_FEAT_NAMED_SHM | PROTON_FEAT_SOCKETPAIR |
                        PROTON_FEAT_EVENTFD | PROTON_FEAT_EPOLL |
                        PROTON_FEAT_PIPE | PROTON_FEAT_MEMFD;
    soft_inc(&g_u64SoftQueryOk);
    /* Wave 104: one-shot greppable soft inventory after first query. */
    soft_inventory_maybe_once();
    return 0;
}

static int
fd_in_range(uint64_t u64Fd)
{
    return u64Fd < (uint64_t)PR_MAX_FD;
}

static int
fd_live(uint64_t u64Fd)
{
    if (!fd_in_range(u64Fd)) {
        return 0;
    }
    return g_aFd[u64Fd].u8Kind != PR_KIND_FREE;
}

static int
fd_ok(uint64_t u64Fd)
{
    fd_table_init();
    return fd_live(u64Fd);
}

static int64_t
alloc_fd_kind(uint8_t u8Kind)
{
    int nFd;

    fd_table_init();
    for (nFd = 3; nFd < PR_MAX_FD; nFd++) {
        if (g_aFd[nFd].u8Kind == PR_KIND_FREE) {
            soft_zero(&g_aFd[nFd], sizeof(g_aFd[nFd]));
            g_aFd[nFd].u8Kind = u8Kind;
            g_aFd[nFd].u32Peer = (uint32_t)nFd;
            return (int64_t)nFd;
        }
    }
    return -(int64_t)E_MFILE;
}

static void
release_fd(uint64_t u64Fd)
{
    if (!fd_in_range(u64Fd) || u64Fd < 3) {
        return;
    }
    soft_zero(&g_aFd[u64Fd], sizeof(g_aFd[u64Fd]));
}

static int64_t
alloc_pair(uint8_t u8KindA, uint8_t u8KindB, int32_t *pFd0, int32_t *pFd1)
{
    int64_t i64A;
    int64_t i64B;

    i64A = alloc_fd_kind(u8KindA);
    if (i64A < 0) {
        return i64A;
    }
    i64B = alloc_fd_kind(u8KindB);
    if (i64B < 0) {
        release_fd((uint64_t)i64A);
        return i64B;
    }
    g_aFd[i64A].u32Peer = (uint32_t)i64B;
    g_aFd[i64B].u32Peer = (uint32_t)i64A;
    *pFd0 = (int32_t)i64A;
    *pFd1 = (int32_t)i64B;
    return 0;
}

/*
 * uname — G-UNAME-1: sysname=Linux; release/version include greenjade.
 * Buffer layout matches Linux struct utsname (6 × 65-byte fields).
 */
static int64_t
cold_uname(uint64_t u64Buf)
{
    char *p;
    unsigned i;

    if (u64Buf == 0) {
        return -(int64_t)E_FAULT;
    }
    p = (char *)(uintptr_t)u64Buf;
    /* Kernel smoke may pass a kernel pointer; full path uses copy_to_user. */
    for (i = 0; i < 6u * 65u; i++) {
        p[i] = 0;
    }
    {
        const char *szSys = "Linux";
        const char *szNode = "greenjade";
        const char *szRel = "6.0.0-greenjade";
        const char *szVer = "#1 SMP greenjade";
        const char *szMach = "x86_64";
        const char *szDom = "(none)";
        const char *asz[6];
        unsigned iField;

        asz[0] = szSys;
        asz[1] = szNode;
        asz[2] = szRel;
        asz[3] = szVer;
        asz[4] = szMach;
        asz[5] = szDom;
        for (iField = 0; iField < 6; iField++) {
            const char *sz = asz[iField];
            char *pDst = p + iField * 65;
            unsigned j;

            for (j = 0; sz[j] != 0 && j < 64; j++) {
                pDst[j] = sz[j];
            }
        }
    }
    return 0;
}

static int64_t
cold_getrandom(uint64_t u64Buf, uint64_t u64Len, uint64_t u64Flags)
{
    uint8_t *p;
    uint64_t i;

    (void)u64Flags;
    if (u64Buf == 0) {
        return -(int64_t)E_FAULT;
    }
    if (u64Len > 256) {
        u64Len = 256;
    }
    p = (uint8_t *)(uintptr_t)u64Buf;
    for (i = 0; i < u64Len; i++) {
        g_u32Prng = g_u32Prng * 1664525u + 1013904223u;
        p[i] = (uint8_t)(g_u32Prng >> 16);
    }
    return (int64_t)u64Len;
}

/* Soft mono clock step (host has no timer; advance on query/sleep). */
static void
soft_tick(uint64_t u64Ns)
{
    g_u64MonoNs += u64Ns;
    if (u64Ns == 0) {
        g_u64MonoNs += 1000ull;
    }
}

static int64_t
cold_clock_gettime(uint64_t u64Clk, uint64_t u64Ts)
{
    int64_t *pTs;

    (void)u64Clk; /* CLOCK_REALTIME/MONOTONIC both synthetic mono */
    if (u64Ts == 0) {
        return -(int64_t)E_FAULT;
    }
    soft_tick(0);
    pTs = (int64_t *)(uintptr_t)u64Ts;
    pTs[0] = (int64_t)(g_u64MonoNs / 1000000000ull);
    pTs[1] = (int64_t)(g_u64MonoNs % 1000000000ull);
    return 0;
}

static int64_t
cold_clock_getres(uint64_t u64Clk, uint64_t u64Ts)
{
    int64_t *pTs;

    (void)u64Clk;
    if (u64Ts == 0) {
        return 0;
    }
    pTs = (int64_t *)(uintptr_t)u64Ts;
    pTs[0] = 0;
    pTs[1] = 1000; /* 1 us soft resolution */
    return 0;
}

/*
 * Minimal Linux stat layout fields used by probes (st_mode + st_size).
 * Full struct is larger; we zero 144 bytes (x86_64 stat) when possible.
 */
static int64_t
cold_fill_stat(uint64_t u64Buf, uint32_t u32Mode, uint64_t u64Size)
{
    uint8_t *p;
    uint32_t *pMode;
    uint64_t *pSize;

    if (u64Buf == 0) {
        return -(int64_t)E_FAULT;
    }
    p = (uint8_t *)(uintptr_t)u64Buf;
    soft_zero(p, 144);
    /* x86_64 struct stat: st_mode @ 24, st_size @ 48 (glibc-ish). */
    pMode = (uint32_t *)(void *)(p + 24);
    pSize = (uint64_t *)(void *)(p + 48);
    *pMode = u32Mode;
    *pSize = u64Size;
    return 0;
}

static uint32_t
soft_poll_mask(uint64_t u64Fd, uint32_t u32Want)
{
    uint8_t u8Kind;

    if (!fd_ok(u64Fd)) {
        return PR_POLLERR;
    }
    u8Kind = g_aFd[u64Fd].u8Kind;
    switch (u8Kind) {
    case PR_KIND_STDIO:
        /* stdin readable soft; stdout/err writable */
        if (u64Fd == 0) {
            return u32Want & PR_POLLIN;
        }
        return u32Want & PR_POLLOUT;
    case PR_KIND_PIPE_R:
    case PR_KIND_SIGNALFD:
        return u32Want & PR_POLLIN; /* soft always readable (EOF/empty ok) */
    case PR_KIND_PIPE_W:
        return u32Want & PR_POLLOUT;
    case PR_KIND_EVENTFD:
        if (g_aFd[u64Fd].u64Counter != 0) {
            return u32Want & (PR_POLLIN | PR_POLLOUT);
        }
        return u32Want & PR_POLLOUT;
    case PR_KIND_SOCKET:
        return u32Want & (PR_POLLIN | PR_POLLOUT);
    case PR_KIND_TIMERFD:
        if (g_aFd[u64Fd].u64Counter != 0) {
            return u32Want & PR_POLLIN;
        }
        return 0;
    case PR_KIND_OPEN:
    case PR_KIND_MEMFD:
        return u32Want & (PR_POLLIN | PR_POLLOUT);
    case PR_KIND_EPOLL:
        return 0;
    default:
        return PR_POLLERR;
    }
}

static int64_t
cold_poll(uint64_t u64Fds, uint64_t u64Nfds, uint64_t u64Timeout)
{
    uint32_t cNfds;
    uint32_t i;
    uint32_t cReady;

    (void)u64Timeout;
    if (u64Nfds == 0) {
        return 0;
    }
    if (u64Fds == 0) {
        return -(int64_t)E_FAULT;
    }
    cNfds = (uint32_t)u64Nfds;
    if (cNfds > 16u) {
        cNfds = 16u;
    }
    cReady = 0;
    for (i = 0; i < cNfds; i++) {
        /* pollfd: int fd; short events; short revents — 8 bytes */
        uint8_t *pBase = (uint8_t *)(uintptr_t)(u64Fds + (uint64_t)i * 8u);
        int32_t nFd;
        uint16_t u16Events;
        uint16_t u16Revents;
        uint32_t u32Got;

        soft_copy(&nFd, pBase, 4);
        soft_copy(&u16Events, pBase + 4, 2);
        if (nFd < 0) {
            u16Revents = 0;
        } else {
            uint32_t u32Want = (uint32_t)u16Events;

            if (u32Want == 0) {
                u32Want = PR_POLLIN | PR_POLLOUT;
            }
            u32Got = soft_poll_mask((uint64_t)nFd, u32Want);
            u16Revents = (uint16_t)u32Got;
        }
        soft_copy(pBase + 6, &u16Revents, 2);
        if (u16Revents != 0) {
            cReady++;
        }
    }
    return (int64_t)cReady;
}

static int64_t
cold_select(uint64_t u64Nfds, uint64_t u64Read, uint64_t u64Write,
            uint64_t u64Except)
{
    uint32_t cNfds;
    uint32_t i;
    uint32_t cReady;
    uint64_t aRead[1];
    uint64_t aWrite[1];
    uint64_t u64ROut;
    uint64_t u64WOut;
    uint64_t u64EOut;

    if (u64Nfds == 0) {
        return 0;
    }
    cNfds = (uint32_t)u64Nfds;
    if (cNfds > 64u) {
        cNfds = 64u;
    }
    aRead[0] = 0;
    aWrite[0] = 0;
    if (u64Read != 0) {
        soft_copy(aRead, (const void *)(uintptr_t)u64Read, 8);
    }
    if (u64Write != 0) {
        soft_copy(aWrite, (const void *)(uintptr_t)u64Write, 8);
    }
    u64ROut = 0;
    u64WOut = 0;
    u64EOut = 0;
    cReady = 0;
    for (i = 0; i < cNfds; i++) {
        uint64_t u64Bit = 1ull << i;

        if ((aRead[0] & u64Bit) != 0) {
            if ((soft_poll_mask((uint64_t)i, PR_POLLIN) & PR_POLLIN) != 0) {
                u64ROut |= u64Bit;
                cReady++;
            }
        }
        if ((aWrite[0] & u64Bit) != 0) {
            if ((soft_poll_mask((uint64_t)i, PR_POLLOUT) & PR_POLLOUT) != 0) {
                u64WOut |= u64Bit;
                cReady++;
            }
        }
    }
    if (u64Read != 0) {
        soft_copy((void *)(uintptr_t)u64Read, &u64ROut, 8);
    }
    if (u64Write != 0) {
        soft_copy((void *)(uintptr_t)u64Write, &u64WOut, 8);
    }
    if (u64Except != 0) {
        soft_copy((void *)(uintptr_t)u64Except, &u64EOut, 8);
    }
    return (int64_t)cReady;
}

static int64_t
cold_read(uint64_t u64Fd, uint64_t u64Buf, uint64_t u64Len)
{
    uint8_t *p;
    uint64_t cb;

    if (!fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    if (u64Len == 0) {
        return 0;
    }
    if (u64Buf == 0) {
        return -(int64_t)E_FAULT;
    }
    p = (uint8_t *)(uintptr_t)u64Buf;
    switch (g_aFd[u64Fd].u8Kind) {
    case PR_KIND_EVENTFD: {
        uint64_t u64Val;

        if (g_aFd[u64Fd].u64Counter == 0) {
            return -(int64_t)E_AGAIN;
        }
        if (u64Len < 8) {
            return -(int64_t)E_INVAL;
        }
        u64Val = g_aFd[u64Fd].u64Counter;
        g_aFd[u64Fd].u64Counter = 0;
        soft_copy(p, &u64Val, 8);
        return 8;
    }
    case PR_KIND_TIMERFD: {
        uint64_t u64Exp;

        if (g_aFd[u64Fd].u64Counter == 0) {
            return -(int64_t)E_AGAIN;
        }
        if (u64Len < 8) {
            return -(int64_t)E_INVAL;
        }
        u64Exp = 1;
        g_aFd[u64Fd].u64Counter = 0;
        soft_copy(p, &u64Exp, 8);
        return 8;
    }
    case PR_KIND_STDIO:
        if (u64Fd == 0) {
            return 0; /* stdin EOF soft */
        }
        return -(int64_t)E_BADF;
    case PR_KIND_PIPE_R:
    case PR_KIND_SIGNALFD:
    case PR_KIND_OPEN:
    case PR_KIND_MEMFD:
    case PR_KIND_SOCKET:
        /* Soft EOF / empty */
        (void)cb;
        return 0;
    default:
        return -(int64_t)E_BADF;
    }
}

static int64_t
cold_write(uint64_t u64Fd, uint64_t u64Buf, uint64_t u64Len)
{
    if (!fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    if (u64Len == 0) {
        return 0;
    }
    if (u64Buf == 0) {
        return -(int64_t)E_FAULT;
    }
    switch (g_aFd[u64Fd].u8Kind) {
    case PR_KIND_EVENTFD: {
        uint64_t u64Add = 0;

        if (u64Len < 8) {
            return -(int64_t)E_INVAL;
        }
        soft_copy(&u64Add, (const void *)(uintptr_t)u64Buf, 8);
        g_aFd[u64Fd].u64Counter += u64Add;
        return 8;
    }
    case PR_KIND_PIPE_W:
    case PR_KIND_STDIO:
    case PR_KIND_OPEN:
    case PR_KIND_MEMFD:
    case PR_KIND_SOCKET:
        /* Discard payload; report full write for probes. */
        if (u64Len > 0x10000ull) {
            u64Len = 0x10000ull;
        }
        return (int64_t)u64Len;
    case PR_KIND_PIPE_R:
        return -(int64_t)E_BADF;
    default:
        return -(int64_t)E_BADF;
    }
}

static int64_t
cold_writev(uint64_t u64Fd, uint64_t u64Iov, uint64_t u64Iovcnt)
{
    uint64_t cIov;
    uint64_t i;
    int64_t i64Total;

    if (!fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    if (u64Iovcnt == 0) {
        return 0;
    }
    if (u64Iov == 0) {
        return -(int64_t)E_FAULT;
    }
    cIov = u64Iovcnt;
    if (cIov > 16) {
        cIov = 16;
    }
    i64Total = 0;
    for (i = 0; i < cIov; i++) {
        /* iovec: void *iov_base; size_t iov_len — 16 bytes on LP64 */
        uint64_t u64Base = 0;
        uint64_t u64Len = 0;
        uint8_t *pIov = (uint8_t *)(uintptr_t)(u64Iov + i * 16u);
        int64_t i64W;

        soft_copy(&u64Base, pIov, 8);
        soft_copy(&u64Len, pIov + 8, 8);
        i64W = cold_write(u64Fd, u64Base, u64Len);
        if (i64W < 0) {
            return (i64Total > 0) ? i64Total : i64W;
        }
        i64Total += i64W;
    }
    return i64Total;
}

static int64_t
cold_readv(uint64_t u64Fd, uint64_t u64Iov, uint64_t u64Iovcnt)
{
    uint64_t cIov;
    uint64_t i;
    int64_t i64Total;

    if (!fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    if (u64Iovcnt == 0) {
        return 0;
    }
    if (u64Iov == 0) {
        return -(int64_t)E_FAULT;
    }
    cIov = u64Iovcnt;
    if (cIov > 16) {
        cIov = 16;
    }
    i64Total = 0;
    for (i = 0; i < cIov; i++) {
        uint64_t u64Base = 0;
        uint64_t u64Len = 0;
        uint8_t *pIov = (uint8_t *)(uintptr_t)(u64Iov + i * 16u);
        int64_t i64R;

        soft_copy(&u64Base, pIov, 8);
        soft_copy(&u64Len, pIov + 8, 8);
        i64R = cold_read(u64Fd, u64Base, u64Len);
        if (i64R < 0) {
            return (i64Total > 0) ? i64Total : i64R;
        }
        i64Total += i64R;
        if (i64R == 0) {
            break;
        }
    }
    return i64Total;
}

static int64_t
cold_dup3(uint64_t u64Old, uint64_t u64New, uint64_t u64Flags)
{
    int nNew;

    if (!fd_ok(u64Old)) {
        return -(int64_t)E_BADF;
    }
    if (u64Old == u64New) {
        return (int64_t)u64New;
    }
    if (!fd_in_range(u64New) || u64New < 3) {
        return -(int64_t)E_INVAL;
    }
    nNew = (int)u64New;
    if (g_aFd[nNew].u8Kind != PR_KIND_FREE) {
        release_fd((uint64_t)nNew);
    }
    g_aFd[nNew] = g_aFd[u64Old];
    if ((u64Flags & 0x80000ull) != 0) { /* O_CLOEXEC */
        g_aFd[nNew].u8Cloexec = 1;
    }
    return (int64_t)nNew;
}

static int64_t
cold_dup(uint64_t u64Old)
{
    int64_t i64New;

    if (!fd_ok(u64Old)) {
        return -(int64_t)E_BADF;
    }
    i64New = alloc_fd_kind(g_aFd[u64Old].u8Kind);
    if (i64New < 0) {
        return i64New;
    }
    g_aFd[i64New] = g_aFd[u64Old];
    return i64New;
}

static int64_t
cold_fcntl(uint64_t u64Fd, uint64_t u64Cmd, uint64_t u64Arg)
{
    if (!fd_ok(u64Fd) && u64Fd > 2) {
        return -(int64_t)E_BADF;
    }
    if (!fd_ok(u64Fd)) {
        /* stdio always ok after init */
        fd_table_init();
        if (!fd_ok(u64Fd)) {
            return -(int64_t)E_BADF;
        }
    }
    switch ((int)u64Cmd) {
    case PR_F_GETFD:
        return g_aFd[u64Fd].u8Cloexec ? 1 : 0;
    case PR_F_SETFD:
        g_aFd[u64Fd].u8Cloexec = ((u64Arg & 1u) != 0) ? 1u : 0u;
        return 0;
    case PR_F_GETFL:
        return (int64_t)g_aFd[u64Fd].u8Flags;
    case PR_F_SETFL:
        g_aFd[u64Fd].u8Flags = (uint8_t)(u64Arg & 0xffu);
        return 0;
    case PR_F_DUPFD:
    case PR_F_DUPFD_CLOEXEC: {
        int nStart = (int)u64Arg;
        int nFd;

        if (nStart < 3) {
            nStart = 3;
        }
        for (nFd = nStart; nFd < PR_MAX_FD; nFd++) {
            if (g_aFd[nFd].u8Kind == PR_KIND_FREE) {
                g_aFd[nFd] = g_aFd[u64Fd];
                if ((int)u64Cmd == PR_F_DUPFD_CLOEXEC) {
                    g_aFd[nFd].u8Cloexec = 1;
                }
                return (int64_t)nFd;
            }
        }
        return -(int64_t)E_MFILE;
    }
    default:
        /* Soft success for unknown fcntl probes (locks etc.). */
        return 0;
    }
}

static int64_t
cold_ioctl(uint64_t u64Fd, uint64_t u64Cmd, uint64_t u64Arg)
{
    uint32_t u32Cmd;

    if (!fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    u32Cmd = (uint32_t)u64Cmd;
    if (u32Cmd == 0x5413u /* TIOCGWINSZ */) {
        uint16_t aWs[4];

        aWs[0] = 24;
        aWs[1] = 80;
        aWs[2] = 0;
        aWs[3] = 0;
        if (u64Arg != 0) {
            soft_copy((void *)(uintptr_t)u64Arg, aWs, sizeof(aWs));
        }
        return 0;
    }
    if (u32Cmd == 0x541Bu /* FIONREAD */) {
        uint32_t u32Zero = 0;

        if (u64Arg != 0) {
            soft_copy((void *)(uintptr_t)u64Arg, &u32Zero, 4);
        }
        return 0;
    }
    /* Unknown ioctl: soft succeed for wine/libc probes. */
    return 0;
}

static int64_t
cold_epoll_ctl(uint64_t u64Epfd, uint64_t u64Op, uint64_t u64Fd,
               uint64_t u64Event)
{
    int iSlot;
    int iFree;
    uint32_t u32Events;
    uint64_t u64Data;

    if (!fd_ok(u64Epfd) || g_aFd[u64Epfd].u8Kind != PR_KIND_EPOLL) {
        return -(int64_t)E_BADF;
    }
    if ((int)u64Op != PR_EPOLL_CTL_DEL && !fd_ok(u64Fd)) {
        return -(int64_t)E_BADF;
    }
    u32Events = PR_POLLIN;
    u64Data = 0;
    if (u64Event != 0 && (int)u64Op != PR_EPOLL_CTL_DEL) {
        soft_copy(&u32Events, (const void *)(uintptr_t)u64Event, 4);
        soft_copy(&u64Data, (const void *)(uintptr_t)(u64Event + 4), 8);
        (void)u64Data;
    }
    iFree = -1;
    for (iSlot = 0; iSlot < PR_EPOLL_SLOTS; iSlot++) {
        if (g_aFd[u64Epfd].aEpollFd[iSlot] == (int32_t)u64Fd) {
            if ((int)u64Op == PR_EPOLL_CTL_DEL) {
                g_aFd[u64Epfd].aEpollFd[iSlot] = -1;
                g_aFd[u64Epfd].u32EpollMask[iSlot] = 0;
                return 0;
            }
            if ((int)u64Op == PR_EPOLL_CTL_ADD) {
                return -(int64_t)E_EXIST;
            }
            /* MOD */
            g_aFd[u64Epfd].u32EpollMask[iSlot] = u32Events;
            return 0;
        }
        if (g_aFd[u64Epfd].aEpollFd[iSlot] < 0 && iFree < 0) {
            iFree = iSlot;
        }
    }
    if ((int)u64Op == PR_EPOLL_CTL_DEL) {
        return -(int64_t)E_NOENT;
    }
    if ((int)u64Op == PR_EPOLL_CTL_ADD) {
        if (iFree < 0) {
            return -(int64_t)E_NOMEM;
        }
        g_aFd[u64Epfd].aEpollFd[iFree] = (int32_t)u64Fd;
        g_aFd[u64Epfd].u32EpollMask[iFree] = u32Events;
        return 0;
    }
    return -(int64_t)E_NOENT;
}

static int64_t
cold_epoll_wait(uint64_t u64Epfd, uint64_t u64Events, uint64_t u64Max)
{
    int nMax;
    int iSlot;
    int cOut;

    if (!fd_ok(u64Epfd) || g_aFd[u64Epfd].u8Kind != PR_KIND_EPOLL) {
        return -(int64_t)E_BADF;
    }
    if (u64Max == 0) {
        return 0;
    }
    if (u64Events == 0) {
        return -(int64_t)E_FAULT;
    }
    nMax = (int)u64Max;
    if (nMax > PR_EPOLL_SLOTS) {
        nMax = PR_EPOLL_SLOTS;
    }
    cOut = 0;
    for (iSlot = 0; iSlot < PR_EPOLL_SLOTS && cOut < nMax; iSlot++) {
        int32_t nFd = g_aFd[u64Epfd].aEpollFd[iSlot];
        uint32_t u32Want;
        uint32_t u32Got;
        uint8_t *pEv;

        if (nFd < 0) {
            continue;
        }
        u32Want = g_aFd[u64Epfd].u32EpollMask[iSlot];
        if (u32Want == 0) {
            u32Want = PR_POLLIN;
        }
        u32Got = soft_poll_mask((uint64_t)nFd, u32Want);
        if (u32Got == 0) {
            continue;
        }
        /* epoll_event: uint32 events; uint64 data — 12 bytes packed */
        pEv = (uint8_t *)(uintptr_t)(u64Events + (uint64_t)cOut * 12u);
        soft_copy(pEv, &u32Got, 4);
        {
            uint64_t u64Data = (uint64_t)(uint32_t)nFd;

            soft_copy(pEv + 4, &u64Data, 8);
        }
        cOut++;
    }
    return (int64_t)cOut;
}

static int64_t
cold_prlimit64(uint64_t u64Pid, uint64_t u64Res, uint64_t u64New,
               uint64_t u64Old)
{
    (void)u64Pid;
    (void)u64Res;
    (void)u64New; /* ignore set during bring-up */
    if (u64Old != 0) {
        /* struct rlimit64 { rlim_cur; rlim_max; } */
        uint64_t aLim[2];

        aLim[0] = 0x100000ull; /* soft generous */
        aLim[1] = 0x100000ull;
        soft_copy((void *)(uintptr_t)u64Old, aLim, 16);
    }
    return 0;
}

static int64_t
cold_sysinfo(uint64_t u64Info)
{
    uint8_t *p;

    if (u64Info == 0) {
        return -(int64_t)E_FAULT;
    }
    /* Linux sysinfo is ~112 bytes; zero and set uptime + mem soft. */
    p = (uint8_t *)(uintptr_t)u64Info;
    soft_zero(p, 112);
    {
        int64_t i64Up = (int64_t)(g_u64MonoNs / 1000000000ull);
        uint64_t u64Ram = 256ull * 1024ull * 1024ull;

        soft_copy(p + 0, &i64Up, 8);
        soft_copy(p + 32, &u64Ram, 8); /* totalram-ish soft offset */
    }
    return 0;
}

int64_t
protonrt_cold_linux(uint64_t u64Nr, uint64_t a0, uint64_t a1, uint64_t a2,
                    uint64_t a3, uint64_t a4, uint64_t a5)
{
    uint32_t u32Grp;

    (void)a5;

    fd_table_init();

    /* Wave 104 soft enter — never rewrites ret. greppable: protonrt: soft */
    u32Grp = soft_classify_nr(u64Nr);
    g_u64SoftLastNr = u64Nr;
    g_u32SoftLastGrp = u32Grp;
    if (u32Grp < (uint32_t)COLD_SOFT_GRP_N) {
        soft_inc(&g_aSoftEnter[u32Grp]);
    }
    soft_inc(&g_u64SoftEnterTotal);
    soft_inventory_maybe_once();

    switch (u64Nr) {
    /* ---- file open / close / seek / write -------------------------- */
    case NR_openat:
        if (a1 == 0) {
            return -(int64_t)E_FAULT;
        }
        return alloc_fd_kind(PR_KIND_OPEN);
    case NR_open:
    case NR_creat:
        if (a0 == 0) {
            return -(int64_t)E_FAULT;
        }
        return alloc_fd_kind(PR_KIND_OPEN);
    case NR_memfd_create:
        /* Name optional; flags ignored on soft path. */
        return alloc_fd_kind(PR_KIND_MEMFD);
    case NR_read:
    case NR_pread64:
        return cold_read(a0, a1, a2);
    case NR_write:
    case NR_pwrite64:
        return cold_write(a0, a1, a2);
    case NR_readv:
        return cold_readv(a0, a1, a2);
    case NR_writev:
        return cold_writev(a0, a1, a2);
    case NR_close:
        if (a0 < 3) {
            return 0; /* leave stdio open */
        }
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        release_fd(a0);
        return 0;
    case NR_close_range: {
        uint32_t u32First = (uint32_t)a0;
        uint32_t u32Last = (uint32_t)a1;
        uint32_t i;

        if (u32First > u32Last) {
            return -(int64_t)E_INVAL;
        }
        if (u32Last >= (uint32_t)PR_MAX_FD) {
            u32Last = (uint32_t)PR_MAX_FD - 1u;
        }
        for (i = u32First; i <= u32Last; i++) {
            if (i >= 3u && fd_live((uint64_t)i)) {
                release_fd((uint64_t)i);
            }
        }
        return 0;
    }
    case NR_lseek:
        if (!fd_ok(a0) && a0 > 2) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_ftruncate:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_ioctl:
        return cold_ioctl(a0, a1, a2);
    case NR_fcntl:
        return cold_fcntl(a0, a1, a2);
    case NR_flock:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0; /* soft lock always succeeds */
    case NR_fsync:
    case NR_fdatasync:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_sync:
        return 0;
    case NR_syncfs:
        if (!fd_ok(a0) && (int64_t)a0 < 0) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_madvise:
        return 0; /* advise only */
    case NR_mprotect:
    case NR_munmap:
        /* Hot path owns real maps; soft no-op if cold-routed. */
        return 0;
    case NR_brk:
        /* Soft: report current soft break as success with arg. */
        return (int64_t)a0;

    /* ---- stat / access --------------------------------------------- */
    case NR_fstat:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return cold_fill_stat(a1, 0100644u, 0); /* S_IFREG|0644 soft */
    case NR_stat:
    case NR_lstat:
    case NR_newfstatat:
        /* Soft: any non-null path looks like a regular empty file. */
        if ((u64Nr == NR_newfstatat && a1 == 0) ||
            (u64Nr != NR_newfstatat && a0 == 0)) {
            return -(int64_t)E_FAULT;
        }
        return cold_fill_stat((u64Nr == NR_newfstatat) ? a2 : a1, 0100644u, 0);
    case NR_statx: {
        uint8_t *p;

        if (a1 == 0 || a4 == 0) {
            return -(int64_t)E_FAULT;
        }
        p = (uint8_t *)(uintptr_t)a4;
        soft_zero(p, 256);
        {
            uint32_t u32Mask = 0x000007ffu; /* STATX_BASIC_STATS soft */
            uint16_t u16Mode = 0100644u;

            soft_copy(p + 0, &u32Mask, 4);
            soft_copy(p + 20, &u16Mode, 2);
        }
        return 0;
    }
    case NR_access:
    case NR_faccessat:
        /* Soft allow path probes (A0 install path checks). */
        if ((u64Nr == NR_access && a0 == 0) ||
            (u64Nr == NR_faccessat && a1 == 0)) {
            return -(int64_t)E_FAULT;
        }
        return 0;
    case NR_getdents:
    case NR_getdents64:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0; /* empty directory EOF */

    /* ---- path helpers ---------------------------------------------- */
    case NR_getcwd: {
        char *p;
        unsigned i;

        if (a0 == 0 || a1 < 2) {
            return -(int64_t)E_INVAL;
        }
        p = (char *)(uintptr_t)a0;
        for (i = 0; g_szCwd[i] != 0 && (uint64_t)i + 1 < a1; i++) {
            p[i] = g_szCwd[i];
        }
        p[i] = 0;
        return (int64_t)a0;
    }
    case NR_chdir:
        if (a0 == 0) {
            return -(int64_t)E_FAULT;
        }
        /* Soft: only accept "/" length-1 path */
        g_szCwd[0] = '/';
        g_szCwd[1] = 0;
        return 0;
    case NR_fchdir:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_mkdir:
    case NR_mkdirat:
    case NR_rmdir:
    case NR_unlink:
    case NR_unlinkat:
    case NR_rename:
    case NR_renameat:
    case NR_link:
    case NR_linkat:
    case NR_symlink:
    case NR_symlinkat:
    case NR_chmod:
    case NR_utimensat:
        /* Soft success for install-layout probes (no durable store). */
        return 0;
    case NR_readlink:
    case NR_readlinkat: {
        const char *szFake = "/usr/bin/greenjade";
        char *pDst;
        uint64_t u64Buf;
        uint64_t u64Len;
        unsigned i;

        u64Buf = (u64Nr == NR_readlinkat) ? a2 : a1;
        u64Len = (u64Nr == NR_readlinkat) ? a3 : a2;
        if (u64Buf == 0 || u64Len == 0) {
            return -(int64_t)E_FAULT;
        }
        pDst = (char *)(uintptr_t)u64Buf;
        for (i = 0; szFake[i] != 0 && (uint64_t)i < u64Len; i++) {
            pDst[i] = szFake[i];
        }
        return (int64_t)i;
    }

    /* ---- identity / entropy / limits / time ------------------------ */
    case NR_uname:
        return cold_uname(a0);
    case NR_getrandom:
        return cold_getrandom(a0, a1, a2);
    case NR_prlimit64:
        return cold_prlimit64(a0, a1, a2, a3);
    case NR_getpid:
    case NR_gettid:
        return 1;
    case NR_getppid:
        return 0;
    case NR_getuid:
    case NR_geteuid:
    case NR_getgid:
    case NR_getegid:
        return 0;
    case NR_getpgrp:
    case NR_getsid:
        return 1;
    case NR_setpgid:
    case NR_setsid:
        return 0;
    case NR_set_tid_address:
        g_u64ClearChildTid = a0;
        return 1; /* return tid */
    case NR_personality:
        return 0; /* PER_LINUX */
    case NR_prctl:
    case NR_arch_prctl:
        return 0; /* soft success for TLS/name probes */
    case NR_sched_yield:
        return 0;
    case NR_sched_getscheduler:
        return 0; /* SCHED_NORMAL */
    case NR_sched_getparam:
        if (a1 != 0) {
            int32_t nPrio = 0;

            soft_copy((void *)(uintptr_t)a1, &nPrio, 4);
        }
        return 0;
    case NR_sched_setscheduler:
    case NR_sched_setparam:
        return 0;
    case NR_sched_get_priority_max:
        return 0;
    case NR_sched_get_priority_min:
        return 0;
    case NR_clock_gettime:
        return cold_clock_gettime(a0, a1);
    case NR_clock_getres:
        return cold_clock_getres(a0, a1);
    case NR_clock_nanosleep:
    case NR_nanosleep:
        soft_tick(1000000ull);
        return 0;
    case NR_sysinfo:
        return cold_sysinfo(a0);
    case NR_getrusage:
        if (a1 == 0) {
            return -(int64_t)E_FAULT;
        }
        soft_zero((void *)(uintptr_t)a1, 144);
        return 0;

    /* ---- pipes / event / poll / epoll ------------------------------ */
    case NR_pipe:
    case NR_pipe2: {
        int32_t aFds[2];
        int64_t i64St;

        if (a0 == 0) {
            return -(int64_t)E_FAULT;
        }
        i64St = alloc_pair(PR_KIND_PIPE_R, PR_KIND_PIPE_W, &aFds[0], &aFds[1]);
        if (i64St < 0) {
            return i64St;
        }
        if ((u64Nr == NR_pipe2) && ((a1 & 0x80000ull) != 0)) {
            g_aFd[aFds[0]].u8Cloexec = 1;
            g_aFd[aFds[1]].u8Cloexec = 1;
        }
        soft_copy((void *)(uintptr_t)a0, aFds, 8);
        return 0;
    }
    case NR_eventfd:
    case NR_eventfd2: {
        int64_t i64Fd = alloc_fd_kind(PR_KIND_EVENTFD);

        if (i64Fd < 0) {
            return i64Fd;
        }
        g_aFd[i64Fd].u64Counter = a0;
        if ((u64Nr == NR_eventfd2) && ((a1 & 0x80000ull) != 0)) {
            g_aFd[i64Fd].u8Cloexec = 1;
        }
        return i64Fd;
    }
    case NR_timerfd_create: {
        int64_t i64Fd = alloc_fd_kind(PR_KIND_TIMERFD);

        if (i64Fd < 0) {
            return i64Fd;
        }
        return i64Fd;
    }
    case NR_timerfd_settime:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_TIMERFD) {
            return -(int64_t)E_BADF;
        }
        /* Soft: arm with counter=1 so next read/poll sees ready. */
        g_aFd[a0].u64Counter = 1;
        return 0;
    case NR_timerfd_gettime:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_TIMERFD) {
            return -(int64_t)E_BADF;
        }
        if (a1 != 0) {
            int64_t aIt[4];

            soft_zero(aIt, sizeof(aIt));
            if (g_aFd[a0].u64Counter != 0) {
                aIt[2] = 0;
                aIt[3] = 1;
            }
            soft_copy((void *)(uintptr_t)a1, aIt, sizeof(aIt));
        }
        return 0;
    case NR_signalfd4: {
        int64_t i64Fd;

        if ((int64_t)a0 >= 0 && fd_ok(a0) &&
            g_aFd[a0].u8Kind == PR_KIND_SIGNALFD) {
            return (int64_t)a0; /* update existing soft */
        }
        i64Fd = alloc_fd_kind(PR_KIND_SIGNALFD);
        return i64Fd;
    }
    case NR_epoll_create:
    case NR_epoll_create1: {
        int64_t i64Fd = alloc_fd_kind(PR_KIND_EPOLL);
        int i;

        if (i64Fd < 0) {
            return i64Fd;
        }
        for (i = 0; i < PR_EPOLL_SLOTS; i++) {
            g_aFd[i64Fd].aEpollFd[i] = -1;
            g_aFd[i64Fd].u32EpollMask[i] = 0;
        }
        return i64Fd;
    }
    case NR_epoll_ctl:
        return cold_epoll_ctl(a0, a1, a2, a3);
    case NR_epoll_wait:
    case NR_epoll_pwait:
        return cold_epoll_wait(a0, a1, a2);
    case NR_poll:
    case NR_ppoll:
        return cold_poll(a0, a1, a2);
    case NR_select:
    case NR_pselect6:
        return cold_select(a0, a1, a2, a3);

    /* ---- sockets (soft loopback-shaped) ---------------------------- */
    case NR_socket: {
        int64_t i64Fd = alloc_fd_kind(PR_KIND_SOCKET);

        return i64Fd;
    }
    case NR_socketpair: {
        int32_t aFds[2];
        int64_t i64St;

        if (a3 == 0) {
            return -(int64_t)E_FAULT;
        }
        /* Soft: AF_UNIX-shaped pair regardless of domain for A0. */
        i64St = alloc_pair(PR_KIND_SOCKET, PR_KIND_SOCKET, &aFds[0], &aFds[1]);
        if (i64St < 0) {
            return i64St;
        }
        soft_copy((void *)(uintptr_t)a3, aFds, 8);
        return 0;
    }
    case NR_bind:
    case NR_listen:
    case NR_connect:
    case NR_shutdown:
    case NR_setsockopt:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        return 0;
    case NR_getsockopt:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        if (a3 != 0 && a4 != 0) {
            int32_t nZero = 0;
            int32_t nLen = 4;

            soft_copy((void *)(uintptr_t)a3, &nZero, 4);
            soft_copy((void *)(uintptr_t)a4, &nLen, 4);
        }
        return 0;
    case NR_getsockname:
    case NR_getpeername:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        if (a1 != 0 && a2 != 0) {
            uint16_t aSa[8];
            int32_t nLen = 16;

            soft_zero(aSa, sizeof(aSa));
            aSa[0] = 1; /* AF_UNIX soft */
            soft_copy((void *)(uintptr_t)a1, aSa, 16);
            soft_copy((void *)(uintptr_t)a2, &nLen, 4);
        }
        return 0;
    case NR_accept: {
        int64_t i64Fd;

        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        i64Fd = alloc_fd_kind(PR_KIND_SOCKET);
        return i64Fd;
    }
    case NR_sendto:
    case NR_sendmsg:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        /* Soft: report full send length (a2 for sendto; 0 for sendmsg empty). */
        if (u64Nr == NR_sendto) {
            if (a2 > 0x10000ull) {
                return (int64_t)0x10000ull;
            }
            return (int64_t)a2;
        }
        return 0;
    case NR_recvfrom:
    case NR_recvmsg:
        if (!fd_ok(a0) || g_aFd[a0].u8Kind != PR_KIND_SOCKET) {
            return -(int64_t)E_NOTSOCK;
        }
        return 0; /* soft empty */

    /* ---- process / signal (spawn is native GJ) --------------------- */
    case NR_clone:
    case NR_clone3:
    case NR_fork:
    case NR_execve:
    case NR_wait4:
    case NR_kill:
    case NR_exit:
        return cold_enosys();
    case NR_rt_sigaction:
    case NR_rt_sigprocmask:
        return 0; /* no-op success for early libc probes */
    case NR_dup:
        return cold_dup(a0);
    case NR_dup2:
        return cold_dup3(a0, a1, 0);
    case NR_dup3:
        return cold_dup3(a0, a1, a2);

    /*
     * io_uring — not implemented in this library.
     * Product path: kernel min rings (setup/enter/register) handle these
     * in protonrt_cold_link before fall-through; setup is not ENOSYS
     * there. Host / direct calls of this entry still get ENOSYS.
     */
    case NR_io_uring_setup:
    case NR_io_uring_enter:
    case NR_io_uring_register:
        return cold_enosys();

    /* ---- default: any unlisted NR still ENOSYS --------------------- */
    default:
        return cold_enosys();
    }
}
