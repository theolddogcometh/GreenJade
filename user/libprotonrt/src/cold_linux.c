/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room cold-path Linux syscall stubs (Option C personality).
 * Pure C11. Dual license: MIT OR Apache-2.0 — no GPL source.
 *
 * Role
 * ----
 * Minimal host / bring-up implementations for syscalls that are not on
 * the kernel hot table. Depth grows with the Deck Top 50 matrix only.
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
 */
#include <stdint.h>
#include "../include/protonrt/protonrt.h"

/* Linux x86_64 syscall numbers used on the cold path. */
#define NR_read          0
#define NR_open          2
#define NR_close         3
#define NR_creat        85
#define NR_stat          4
#define NR_fstat         5
#define NR_lstat         6
#define NR_poll          7
#define NR_lseek         8
#define NR_ioctl        16
#define NR_access       21
#define NR_pipe         22
#define NR_select       23
#define NR_dup          32
#define NR_dup2         33
#define NR_socket       41
#define NR_connect      42
#define NR_accept       43
#define NR_sendto       44
#define NR_recvfrom     45
#define NR_bind         49
#define NR_listen       50
#define NR_clone        56
#define NR_fork         57
#define NR_execve       59
#define NR_wait4        61
#define NR_kill         62
#define NR_uname        63
#define NR_fcntl        72
#define NR_getcwd       79
#define NR_chdir        80
#define NR_mkdir        83
#define NR_rmdir        84
#define NR_unlink       87
#define NR_readlink     89
#define NR_getpgrp     111
#define NR_rt_sigaction 13
#define NR_rt_sigprocmask 14
#define NR_epoll_wait  232
#define NR_epoll_ctl   233
#define NR_openat      257
#define NR_mkdirat     258
#define NR_newfstatat  262
#define NR_unlinkat    263
#define NR_faccessat   269
#define NR_pselect6    270
#define NR_ppoll       271
#define NR_epoll_pwait 281
#define NR_eventfd2    290
#define NR_epoll_create1 291
#define NR_pipe2       293
#define NR_prlimit64   302
#define NR_getrandom   318
#define NR_statx       332
#define NR_io_uring_setup    425
#define NR_io_uring_enter    426
#define NR_io_uring_register 427
#define NR_clone3      435

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
#define E_NOSYS   38

/* Minimal fd table for cold open/read/close smoke (stdio 0–2 reserved). */
#define PR_MAX_FD 16
static uint8_t g_aFdUsed[PR_MAX_FD];
static uint32_t g_u32Prng = 0xC0FFEEu;

/*
 * Canonical cold-path "not implemented here" result.
 * Prefer this over scattering bare -E_NOSYS so host and kernel-smoke
 * fall-throughs stay one contract.
 */
static int64_t
cold_enosys(void)
{
    return -(int64_t)E_NOSYS;
}

int
proton_rt_query(struct proton_rt_info *pOut)
{
    if (pOut == 0) {
        return -1;
    }
    /* Packed version: (major << 16) | minor — keep in sync with header. */
    pOut->u32Version =
        ((uint32_t)PROTON_RT_VERSION_MAJOR << 16) | (uint32_t)PROTON_RT_VERSION_MINOR;
    /*
     * Feature mask (libprotonrt 1.9). Hot-path bodies live in the kernel;
     * these bits advertise the personality surface the matrix expects.
     */
    pOut->u32Features = PROTON_FEAT_LINUX_COMPAT | PROTON_FEAT_FUTEX |
                        PROTON_FEAT_NAMED_SHM | PROTON_FEAT_SOCKETPAIR |
                        PROTON_FEAT_EVENTFD;
    return 0;
}

static int64_t
alloc_fd(void)
{
    int nFd;

    for (nFd = 3; nFd < PR_MAX_FD; nFd++) {
        if (!g_aFdUsed[nFd]) {
            g_aFdUsed[nFd] = 1;
            return (int64_t)nFd;
        }
    }
    return -(int64_t)E_MFILE;
}

static int
fd_ok(uint64_t u64Fd)
{
    return u64Fd < PR_MAX_FD && g_aFdUsed[u64Fd];
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

int64_t
protonrt_cold_linux(uint64_t u64Nr, uint64_t a0, uint64_t a1, uint64_t a2,
                    uint64_t a3, uint64_t a4, uint64_t a5)
{
    (void)a3;
    (void)a4;
    (void)a5;

    switch (u64Nr) {
    /* ---- file open / close / seek ---------------------------------- */
    case NR_openat:
        /* Path ignored; allocate a fake fd for matrix smoke. */
        if (a1 == 0) {
            return -(int64_t)E_FAULT;
        }
        return alloc_fd();
    case NR_open:
    case NR_creat:
        if (a0 == 0) {
            return -(int64_t)E_FAULT;
        }
        return alloc_fd();
    case NR_read:
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        return 0; /* EOF stub */
    case NR_close:
        if (a0 < 3) {
            return 0; /* leave stdio open */
        }
        if (!fd_ok(a0)) {
            return -(int64_t)E_BADF;
        }
        g_aFdUsed[a0] = 0;
        return 0;
    case NR_lseek:
        if (!fd_ok(a0) && a0 > 2) {
            return -(int64_t)E_BADF;
        }
        return 0;
    case NR_ioctl:
        /* No device ioctls in this stub; kernel cold may handle TTY. */
        return cold_enosys();
    case NR_fcntl:
        if (!fd_ok(a0) && a0 > 2) {
            return -(int64_t)E_BADF;
        }
        return 0;

    /* ---- stat / access (no vfs content yet) ------------------------ */
    case NR_stat:
    case NR_fstat:
    case NR_lstat:
    case NR_newfstatat:
    case NR_statx:
        return -(int64_t)E_NOENT;
    case NR_access:
    case NR_faccessat:
        return -(int64_t)E_NOENT;

    /* ---- path helpers ---------------------------------------------- */
    case NR_getcwd: {
        /* Return "/\0" when the buffer is large enough. */
        char *p;
        if (a0 == 0 || a1 < 2) {
            return -(int64_t)E_INVAL;
        }
        p = (char *)(uintptr_t)a0;
        p[0] = '/';
        p[1] = 0;
        return (int64_t)a0;
    }
    case NR_chdir:
    case NR_mkdir:
    case NR_mkdirat:
    case NR_rmdir:
    case NR_unlink:
    case NR_unlinkat:
        return -(int64_t)E_ROFS; /* no writable vfs on this stub */
    case NR_readlink:
        return -(int64_t)E_NOENT;

    /* ---- identity / entropy / limits ------------------------------- */
    case NR_uname:
        return cold_uname(a0);
    case NR_getrandom:
        /* Prefer hot path with copy_to_user; cold stub for host lib. */
        return cold_getrandom(a0, a1, a2);
    case NR_prlimit64:
        return 0; /* ignore limits during bring-up */

    /* ---- pipes / event / poll (kernel cold path may own these) ----- */
    case NR_pipe:
    case NR_pipe2:
    case NR_eventfd2:
    case NR_epoll_create1:
    case NR_epoll_ctl:
    case NR_epoll_wait:
    case NR_epoll_pwait:
    case NR_poll:
    case NR_ppoll:
    case NR_select:
    case NR_pselect6:
        return cold_enosys();

    /* ---- sockets (netstackd later) --------------------------------- */
    case NR_socket:
    case NR_connect:
    case NR_accept:
    case NR_sendto:
    case NR_recvfrom:
    case NR_bind:
    case NR_listen:
        return cold_enosys();

    /* ---- process / signal (spawn is native GJ) --------------------- */
    case NR_clone:
    case NR_clone3:
    case NR_fork:
    case NR_execve:
    case NR_wait4:
    case NR_kill:
        return cold_enosys();
    case NR_rt_sigaction:
    case NR_rt_sigprocmask:
        return 0; /* no-op success for early libc probes */
    case NR_dup:
    case NR_dup2:
        return cold_enosys();
    case NR_getpgrp:
        return 1;

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
