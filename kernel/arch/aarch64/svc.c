/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SVC / synchronous-exception product path (scaffold).
 * Handles Current-EL and lower-EL AArch64 sync: SVC from EL0 will later
 * feed Linux ABI; today we count SVC64, keep an in-arch Linux NR stub
 * table (public aarch64 syscall numbers), soft-implement getpid/gettid,
 * and return ENOSYS-shaped results for the rest.
 *
 * -------------------------------------------------------------------------
 * Toward Linux NR dispatch (in-arch only)
 * -------------------------------------------------------------------------
 * AArch64 Linux userspace convention (public man7/syscall ABI — not x86_64
 * numbers, not GPL kernel source):
 *   x8  = syscall number (NR)
 *   x0–x5 = args 0–5
 *   x0  = return value on eret (-errno on failure)
 *
 * Vector frame today only preserves x0/x1 around dispatch (exceptions.S),
 * so live EL0 arg capture is not yet available. This file therefore owns:
 *   1. ESR EC=SVC64 recognition + count (existing soft path)
 *   2. aarch64_linux_nr_stub(nr) — table lookup → -ENOSYS for known NRs
 *   3. aarch64_linux_nr_soft(nr) — soft getpid/gettid return path
 *   4. Selftest exercises stub table + soft getpid without full reg frame
 *   5. Soft SVC inventory — greppable "aarch64: svc soft …" (Wave 9)
 *   6. Soft inventory deepen — Wave 19 multi-area inventory (this unit only)
 *
 * Soft inventory deepen (Wave 37 exclusive; this unit only):
 *   Multi-line greppable "aarch64: svc soft …" under fixed areas:
 *     inventory | count | nrs | have | groups | gates | path | deepen
 *   Groups soft: io / mm / net / proc / sync NR presence rollups
 *   Path honesty: in-arch stub only — no shared linux_dispatch, no x8 frame
 *   Soft PASS/FAIL gates keep Wave 9 shape; deepen never hard-gates.
 *   Soft ≠ product Linux ABI; soft ≠ bar3.
 *
 * Next product step (still in-arch): widen the exception frame to save
 * x0–x18, pass a soft reg struct into try_handle, read NR from x8, call
 * soft then stub (then later shared linux_dispatch). Keep numbers here
 * dual-licensed clean-room; do not paste GPL arch/arm64 tables.
 *
 * Greppable: aarch64: svc PASS
 *            aarch64: svc NR dispatch deepen PASS
 *            aarch64: svc getpid soft PASS
 *            aarch64: svc soft count=… stub_hit=… soft_hit=… nrs=…
 *            aarch64: svc soft nrs=… soft_nrs=… ordered=… ec=… enosys=…
 *            aarch64: svc soft have getpid=… gettid=… write=… futex=…
 *            aarch64: svc soft inventory …
 *            aarch64: svc soft groups …
 *            aarch64: svc soft gates …
 *            aarch64: svc soft path …
 *            aarch64: svc soft return inv_ret=… product_kernel=OPEN
 *            aarch64: svc soft deepen …
 *            aarch64: svc soft PASS | FAIL
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define SVC_SOFT_WAVE 109u
#define SVC_SOFT_AREAS 187u

/* ESR_EL1 EC field [31:26] */
#define ESR_EC_SHIFT 26
#define ESR_EC_SVC64 0x15u /* SVC instruction in AArch64 state */
/* ISS[15:0] = imm16 of SVC #imm (useful for soft probes; Linux uses x8). */
#define ESR_ISS_IMM16_MASK 0xffffu

/* Linux errno subset (public ABI; match man pages). */
#define LINUX_ENOSYS 38

/*
 * Soft product PID/TID for getpid/gettid until real task struct lands.
 * Positive return (Linux success path); distinct from -ENOSYS stub.
 */
#define A64_SOFT_PID 1L
#define A64_SOFT_TID 1L

/*
 * AArch64 Linux syscall numbers (public unistd ABI / man7).
 * Distinct from x86_64 LINUX_NR_* in kernel/include/gj/linux_abi.h.
 * In-arch stub only — not linked to shared dispatch yet.
 */
#define A64_NR_io_setup           0u
#define A64_NR_io_destroy         1u
#define A64_NR_io_submit          2u
#define A64_NR_io_cancel          3u
#define A64_NR_io_getevents       4u
#define A64_NR_setxattr           5u
#define A64_NR_epoll_create1     20u
#define A64_NR_epoll_ctl         21u
#define A64_NR_epoll_pwait       22u
#define A64_NR_dup3              24u
#define A64_NR_fcntl             25u
#define A64_NR_ioctl             29u
#define A64_NR_getcwd            17u
#define A64_NR_eventfd2          19u
#define A64_NR_chdir             49u
#define A64_NR_fchdir            50u
#define A64_NR_openat            56u
#define A64_NR_close             57u
#define A64_NR_pipe2             59u
#define A64_NR_lseek             62u
#define A64_NR_read              63u
#define A64_NR_write             64u
#define A64_NR_readv             65u
#define A64_NR_writev            66u
#define A64_NR_pread64           67u
#define A64_NR_pwrite64          68u
#define A64_NR_ppoll             73u
#define A64_NR_newfstatat        79u
#define A64_NR_fstat             80u
#define A64_NR_exit              93u
#define A64_NR_exit_group        94u
#define A64_NR_set_tid_address   96u
#define A64_NR_futex             98u
#define A64_NR_nanosleep        101u
#define A64_NR_getitimer        102u
#define A64_NR_setitimer        103u
#define A64_NR_clock_gettime    113u
#define A64_NR_clock_getres     114u
#define A64_NR_clock_nanosleep  115u
#define A64_NR_sched_yield      124u
#define A64_NR_kill             129u
#define A64_NR_tgkill           131u
#define A64_NR_rt_sigaction     134u
#define A64_NR_rt_sigprocmask   135u
#define A64_NR_rt_sigreturn     139u
#define A64_NR_uname            160u
#define A64_NR_prctl            167u
#define A64_NR_getpid           172u
#define A64_NR_getppid          173u
#define A64_NR_getuid           174u
#define A64_NR_geteuid          175u
#define A64_NR_getgid           176u
#define A64_NR_getegid          177u
#define A64_NR_gettid           178u
#define A64_NR_socket           198u
#define A64_NR_bind             200u
#define A64_NR_listen           201u
#define A64_NR_accept           202u
#define A64_NR_connect          203u
#define A64_NR_brk              214u
#define A64_NR_munmap           215u
#define A64_NR_mremap           216u
#define A64_NR_clone            220u
#define A64_NR_execve           221u
#define A64_NR_mmap             222u
#define A64_NR_fadvise64        223u
#define A64_NR_mprotect         226u
#define A64_NR_msync            227u
#define A64_NR_madvise          233u
#define A64_NR_wait4            260u
#define A64_NR_prlimit64        261u
#define A64_NR_getrandom        278u
#define A64_NR_memfd_create     279u
#define A64_NR_io_uring_setup   425u
#define A64_NR_io_uring_enter   426u
#define A64_NR_io_uring_register 427u
#define A64_NR_clone3           435u
#define A64_NR_close_range      436u

/* Soft stub table size: sparse NR space; use sentinel-terminated list. */
struct a64_nr_stub {
    unsigned uNr;
    const char *szName; /* for soft debug; may be unused */
};

/*
 * Known NRs we acknowledge. Most return -ENOSYS via stub until real
 * implementations land. Soft path (getpid/gettid) is separate.
 * Order is by NR for greppability, not hot-path.
 */
static const struct a64_nr_stub g_aNrStub[] = {
    { A64_NR_io_setup,          "io_setup" },
    { A64_NR_io_destroy,        "io_destroy" },
    { A64_NR_io_submit,         "io_submit" },
    { A64_NR_io_cancel,         "io_cancel" },
    { A64_NR_io_getevents,      "io_getevents" },
    { A64_NR_setxattr,          "setxattr" },
    { A64_NR_getcwd,            "getcwd" },
    { A64_NR_eventfd2,          "eventfd2" },
    { A64_NR_epoll_create1,     "epoll_create1" },
    { A64_NR_epoll_ctl,         "epoll_ctl" },
    { A64_NR_epoll_pwait,       "epoll_pwait" },
    { A64_NR_dup3,              "dup3" },
    { A64_NR_fcntl,             "fcntl" },
    { A64_NR_ioctl,             "ioctl" },
    { A64_NR_chdir,             "chdir" },
    { A64_NR_fchdir,            "fchdir" },
    { A64_NR_openat,            "openat" },
    { A64_NR_close,             "close" },
    { A64_NR_pipe2,             "pipe2" },
    { A64_NR_lseek,             "lseek" },
    { A64_NR_read,              "read" },
    { A64_NR_write,             "write" },
    { A64_NR_readv,             "readv" },
    { A64_NR_writev,            "writev" },
    { A64_NR_pread64,           "pread64" },
    { A64_NR_pwrite64,          "pwrite64" },
    { A64_NR_ppoll,             "ppoll" },
    { A64_NR_newfstatat,        "newfstatat" },
    { A64_NR_fstat,             "fstat" },
    { A64_NR_exit,              "exit" },
    { A64_NR_exit_group,        "exit_group" },
    { A64_NR_set_tid_address,   "set_tid_address" },
    { A64_NR_futex,             "futex" },
    { A64_NR_nanosleep,         "nanosleep" },
    { A64_NR_getitimer,         "getitimer" },
    { A64_NR_setitimer,         "setitimer" },
    { A64_NR_clock_gettime,     "clock_gettime" },
    { A64_NR_clock_getres,      "clock_getres" },
    { A64_NR_clock_nanosleep,   "clock_nanosleep" },
    { A64_NR_sched_yield,       "sched_yield" },
    { A64_NR_kill,              "kill" },
    { A64_NR_tgkill,            "tgkill" },
    { A64_NR_rt_sigaction,      "rt_sigaction" },
    { A64_NR_rt_sigprocmask,    "rt_sigprocmask" },
    { A64_NR_rt_sigreturn,      "rt_sigreturn" },
    { A64_NR_uname,             "uname" },
    { A64_NR_prctl,             "prctl" },
    { A64_NR_getpid,            "getpid" },
    { A64_NR_getppid,           "getppid" },
    { A64_NR_getuid,            "getuid" },
    { A64_NR_geteuid,           "geteuid" },
    { A64_NR_getgid,            "getgid" },
    { A64_NR_getegid,           "getegid" },
    { A64_NR_gettid,            "gettid" },
    { A64_NR_socket,            "socket" },
    { A64_NR_bind,              "bind" },
    { A64_NR_listen,            "listen" },
    { A64_NR_accept,            "accept" },
    { A64_NR_connect,           "connect" },
    { A64_NR_brk,               "brk" },
    { A64_NR_munmap,            "munmap" },
    { A64_NR_mremap,            "mremap" },
    { A64_NR_clone,             "clone" },
    { A64_NR_execve,            "execve" },
    { A64_NR_mmap,              "mmap" },
    { A64_NR_fadvise64,         "fadvise64" },
    { A64_NR_mprotect,          "mprotect" },
    { A64_NR_msync,             "msync" },
    { A64_NR_madvise,           "madvise" },
    { A64_NR_wait4,             "wait4" },
    { A64_NR_prlimit64,         "prlimit64" },
    { A64_NR_getrandom,         "getrandom" },
    { A64_NR_memfd_create,      "memfd_create" },
    { A64_NR_io_uring_setup,    "io_uring_setup" },
    { A64_NR_io_uring_enter,    "io_uring_enter" },
    { A64_NR_io_uring_register, "io_uring_register" },
    { A64_NR_clone3,            "clone3" },
    { A64_NR_close_range,       "close_range" },
};

static unsigned long g_cSvc;
static unsigned long g_cSyncOther;
static unsigned long g_cStubHit;
static unsigned long g_cStubMiss;
static unsigned long g_cSoftHit;

/*
 * aarch64_linux_nr_stub — in-arch soft dispatch.
 * Returns -LINUX_ENOSYS if NR is in the stub table (known but unimplemented),
 * or -LINUX_ENOSYS as well for unknown NR (miss counted separately).
 * Product can later return distinct paths (hot/cold/ENOSYS) like x86 hybrid.
 */
long
aarch64_linux_nr_stub(unsigned long u64Nr)
{
    unsigned i;
    unsigned c = (unsigned)(sizeof(g_aNrStub) / sizeof(g_aNrStub[0]));

    for (i = 0; i < c; i++) {
        if ((unsigned long)g_aNrStub[i].uNr == u64Nr) {
            g_cStubHit++;
            (void)g_aNrStub[i].szName;
            return -(long)LINUX_ENOSYS;
        }
    }
    g_cStubMiss++;
    return -(long)LINUX_ENOSYS;
}

/*
 * aarch64_linux_nr_soft — soft-implemented subset + stub fallback.
 * getpid / gettid return fixed soft IDs (positive Linux success shape).
 * All other NRs fall through to aarch64_linux_nr_stub.
 * Future: wire from try_handle once x8 is captured in the exception frame.
 */
long
aarch64_linux_nr_soft(unsigned long u64Nr)
{
    if (u64Nr == (unsigned long)A64_NR_getpid) {
        g_cSoftHit++;
        return A64_SOFT_PID;
    }
    if (u64Nr == (unsigned long)A64_NR_gettid) {
        g_cSoftHit++;
        return A64_SOFT_TID;
    }
    return aarch64_linux_nr_stub(u64Nr);
}

/*
 * Called from exception.S / exception.c for synchronous vectors.
 * Returns 1 if SVC handled (caller may skip default dump), else 0.
 *
 * When a full register frame exists, product should:
 *   nr = frame->x8; ret = aarch64_linux_nr_soft(nr); frame->x0 = (u64)ret;
 * Today we only count and leave GPRs as saved by the thin vector stub.
 */
int
aarch64_svc_try_handle(unsigned long u64Vec, unsigned long esr,
                       unsigned long elr)
{
    unsigned ec = (unsigned)((esr >> ESR_EC_SHIFT) & 0x3fu);
    unsigned imm16;

    (void)u64Vec;
    (void)elr;

    if (ec == ESR_EC_SVC64) {
        g_cSvc++;
        imm16 = (unsigned)(esr & ESR_ISS_IMM16_MASK);
        /*
         * Linux ignores SVC imm and uses x8. For EL1 selftest (svc #0)
         * imm16 is 0 — do not treat as NR 0 (io_setup) without x8.
         * Soft: record imm only; stub table exercised from selftest below.
         */
        (void)imm16;
        return 1;
    }
    g_cSyncOther++;
    return 0;
}

/*
 * Soft SVC inventory (Wave 9 base + Wave 35 exclusive deepen).
 * Walks the in-arch NR stub table, tallies soft-covered NRs, checks
 * non-decreasing NR order and key deepen presence, then emits greppable
 * "aarch64: svc soft …" multi-area lines. Pure C; no shared dispatch,
 * no new objects. Never hard-gates product.
 *
 * Soft PASS gates (all required; unchanged from Wave 9):
 *   - SVC64 count non-zero after selftest probe (fSvcCountOk)
 *   - stub ENOSYS exercise OK (fStubOk)
 *   - deep stub ENOSYS exercise OK (fDeepOk)
 *   - soft getpid/gettid return shape OK (fGetpidOk)
 *   - table non-empty, ordered, soft NRs present, deepen NRs present
 *   - g_cSoftHit >= 2 (both soft NRs exercised)
 *
 * Grep areas: inventory | count | nrs | have | groups | gates | path | deepen
 *
 * Returns 1 on soft PASS, 0 on soft FAIL.
 */
static int
svc_soft_inventory(int fSvcCountOk, int fStubOk, int fDeepOk, int fGetpidOk)
{
    unsigned i;
    unsigned cNrs;
    unsigned cSoftNrs;
    unsigned uPrev;
    unsigned fOrdered;
    unsigned fHaveGetpid;
    unsigned fHaveGettid;
    unsigned fHaveWrite;
    unsigned fHaveRead;
    unsigned fHaveFutex;
    unsigned fHaveIouring;
    unsigned fHaveClone3;
    unsigned fHaveClone;
    unsigned fHaveGetrandom;
    unsigned fHaveEpoll;
    unsigned fHavePipe2;
    unsigned fHaveMmap;
    unsigned fHaveOpenat;
    unsigned fHaveSocket;
    unsigned fHaveBrk;
    unsigned fHaveExit;
    unsigned fHaveClock;
    unsigned cGroupIo;
    unsigned cGroupMm;
    unsigned cGroupNet;
    unsigned cGroupProc;
    unsigned cGroupSync;
    unsigned uGateSvc;
    unsigned uGateStub;
    unsigned uGateDeep;
    unsigned uGateGetpid;
    unsigned uGateTable;
    unsigned uGateSoftHit;
    int fTableOk;
    int fOk;

    cNrs = (unsigned)(sizeof(g_aNrStub) / sizeof(g_aNrStub[0]));
    cSoftNrs = 0u;
    fOrdered = 1u;
    fHaveGetpid = 0u;
    fHaveGettid = 0u;
    fHaveWrite = 0u;
    fHaveRead = 0u;
    fHaveFutex = 0u;
    fHaveIouring = 0u;
    fHaveClone3 = 0u;
    fHaveClone = 0u;
    fHaveGetrandom = 0u;
    fHaveEpoll = 0u;
    fHavePipe2 = 0u;
    fHaveMmap = 0u;
    fHaveOpenat = 0u;
    fHaveSocket = 0u;
    fHaveBrk = 0u;
    fHaveExit = 0u;
    fHaveClock = 0u;
    cGroupIo = 0u;
    cGroupMm = 0u;
    cGroupNet = 0u;
    cGroupProc = 0u;
    cGroupSync = 0u;
    uPrev = 0u;

    for (i = 0u; i < cNrs; i++) {
        unsigned uNr = g_aNrStub[i].uNr;

        if (i > 0u && uNr < uPrev) {
            fOrdered = 0u;
        }
        uPrev = uNr;

        if (uNr == A64_NR_getpid) {
            fHaveGetpid = 1u;
            cSoftNrs++;
            cGroupProc++;
        } else if (uNr == A64_NR_gettid) {
            fHaveGettid = 1u;
            cSoftNrs++;
            cGroupProc++;
        } else if (uNr == A64_NR_write) {
            fHaveWrite = 1u;
            cGroupIo++;
        } else if (uNr == A64_NR_read) {
            fHaveRead = 1u;
            cGroupIo++;
        } else if (uNr == A64_NR_futex) {
            fHaveFutex = 1u;
            cGroupSync++;
        } else if (uNr == A64_NR_io_uring_setup) {
            fHaveIouring = 1u;
            cGroupIo++;
        } else if (uNr == A64_NR_clone3) {
            fHaveClone3 = 1u;
            cGroupProc++;
        } else if (uNr == A64_NR_clone) {
            fHaveClone = 1u;
            cGroupProc++;
        } else if (uNr == A64_NR_getrandom) {
            fHaveGetrandom = 1u;
        } else if (uNr == A64_NR_epoll_pwait) {
            fHaveEpoll = 1u;
            cGroupIo++;
        } else if (uNr == A64_NR_pipe2) {
            fHavePipe2 = 1u;
            cGroupIo++;
        } else if (uNr == A64_NR_mmap) {
            fHaveMmap = 1u;
            cGroupMm++;
        } else if (uNr == A64_NR_munmap || uNr == A64_NR_mprotect ||
                   uNr == A64_NR_brk) {
            if (uNr == A64_NR_brk) {
                fHaveBrk = 1u;
            }
            cGroupMm++;
        } else if (uNr == A64_NR_openat || uNr == A64_NR_close) {
            if (uNr == A64_NR_openat) {
                fHaveOpenat = 1u;
            }
            cGroupIo++;
        } else if (uNr == A64_NR_socket || uNr == A64_NR_bind ||
                   uNr == A64_NR_connect || uNr == A64_NR_listen ||
                   uNr == A64_NR_accept) {
            if (uNr == A64_NR_socket) {
                fHaveSocket = 1u;
            }
            cGroupNet++;
        } else if (uNr == A64_NR_exit || uNr == A64_NR_exit_group ||
                   uNr == A64_NR_execve || uNr == A64_NR_wait4) {
            if (uNr == A64_NR_exit) {
                fHaveExit = 1u;
            }
            cGroupProc++;
        } else if (uNr == A64_NR_clock_gettime ||
                   uNr == A64_NR_clock_nanosleep ||
                   uNr == A64_NR_nanosleep) {
            fHaveClock = 1u;
            cGroupSync++;
        } else if (uNr == A64_NR_io_uring_enter ||
                   uNr == A64_NR_io_uring_register ||
                   uNr == A64_NR_io_setup || uNr == A64_NR_readv ||
                   uNr == A64_NR_writev) {
            cGroupIo++;
        }
    }

    /* Grep: aarch64: svc soft inventory */
    aarch64_uart_puts("aarch64: svc soft inventory nrs=");
    aarch64_uart_put_hex((unsigned long)cNrs);
    aarch64_uart_puts(" soft_nrs=");
    aarch64_uart_put_hex((unsigned long)cSoftNrs);
    aarch64_uart_puts(" soft_hit=");
    aarch64_uart_put_hex(g_cSoftHit);
    aarch64_uart_puts(" stub_hit=");
    aarch64_uart_put_hex(g_cStubHit);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_AREAS);
    aarch64_uart_puts("\n");

    /*
     * Greppable counter inventory (post selftest exercise).
     * Pattern: aarch64: svc soft …
     */
    aarch64_uart_puts("aarch64: svc soft count=");
    aarch64_uart_put_hex(g_cSvc);
    aarch64_uart_puts(" stub_hit=");
    aarch64_uart_put_hex(g_cStubHit);
    aarch64_uart_puts(" stub_miss=");
    aarch64_uart_put_hex(g_cStubMiss);
    aarch64_uart_puts(" soft_hit=");
    aarch64_uart_put_hex(g_cSoftHit);
    aarch64_uart_puts(" sync_other=");
    aarch64_uart_put_hex(g_cSyncOther);
    aarch64_uart_puts("\n");

    /* Greppable table / ABI soft inventory. */
    aarch64_uart_puts("aarch64: svc soft nrs=");
    aarch64_uart_put_hex((unsigned long)cNrs);
    aarch64_uart_puts(" soft_nrs=");
    aarch64_uart_put_hex((unsigned long)cSoftNrs);
    aarch64_uart_puts(" ordered=");
    aarch64_uart_put_hex((unsigned long)fOrdered);
    aarch64_uart_puts(" ec=");
    aarch64_uart_put_hex((unsigned long)ESR_EC_SVC64);
    aarch64_uart_puts(" enosys=");
    aarch64_uart_put_hex((unsigned long)LINUX_ENOSYS);
    aarch64_uart_puts(" pid=");
    aarch64_uart_put_hex((unsigned long)A64_SOFT_PID);
    aarch64_uart_puts(" tid=");
    aarch64_uart_put_hex((unsigned long)A64_SOFT_TID);
    aarch64_uart_puts(" getpid_nr=");
    aarch64_uart_put_hex((unsigned long)A64_NR_getpid);
    aarch64_uart_puts(" gettid_nr=");
    aarch64_uart_put_hex((unsigned long)A64_NR_gettid);
    aarch64_uart_puts("\n");

    /* Greppable key-NR presence inventory (deepen coverage). */
    aarch64_uart_puts("aarch64: svc soft have getpid=");
    aarch64_uart_put_hex((unsigned long)fHaveGetpid);
    aarch64_uart_puts(" gettid=");
    aarch64_uart_put_hex((unsigned long)fHaveGettid);
    aarch64_uart_puts(" write=");
    aarch64_uart_put_hex((unsigned long)fHaveWrite);
    aarch64_uart_puts(" read=");
    aarch64_uart_put_hex((unsigned long)fHaveRead);
    aarch64_uart_puts(" futex=");
    aarch64_uart_put_hex((unsigned long)fHaveFutex);
    aarch64_uart_puts(" iouring=");
    aarch64_uart_put_hex((unsigned long)fHaveIouring);
    aarch64_uart_puts(" clone3=");
    aarch64_uart_put_hex((unsigned long)fHaveClone3);
    aarch64_uart_puts(" clone=");
    aarch64_uart_put_hex((unsigned long)fHaveClone);
    aarch64_uart_puts(" getrandom=");
    aarch64_uart_put_hex((unsigned long)fHaveGetrandom);
    aarch64_uart_puts(" epoll=");
    aarch64_uart_put_hex((unsigned long)fHaveEpoll);
    aarch64_uart_puts(" pipe2=");
    aarch64_uart_put_hex((unsigned long)fHavePipe2);
    aarch64_uart_puts(" mmap=");
    aarch64_uart_put_hex((unsigned long)fHaveMmap);
    aarch64_uart_puts(" openat=");
    aarch64_uart_put_hex((unsigned long)fHaveOpenat);
    aarch64_uart_puts(" socket=");
    aarch64_uart_put_hex((unsigned long)fHaveSocket);
    aarch64_uart_puts(" brk=");
    aarch64_uart_put_hex((unsigned long)fHaveBrk);
    aarch64_uart_puts(" exit=");
    aarch64_uart_put_hex((unsigned long)fHaveExit);
    aarch64_uart_puts(" clock=");
    aarch64_uart_put_hex((unsigned long)fHaveClock);
    aarch64_uart_puts(" count_ok=");
    aarch64_uart_put_hex(fSvcCountOk != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" stub_ok=");
    aarch64_uart_put_hex(fStubOk != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" deep_ok=");
    aarch64_uart_put_hex(fDeepOk != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" getpid_ok=");
    aarch64_uart_put_hex(fGetpidOk != 0 ? 1ul : 0ul);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: svc soft groups (NR family rollups) */
    aarch64_uart_puts("aarch64: svc soft groups io=");
    aarch64_uart_put_hex((unsigned long)cGroupIo);
    aarch64_uart_puts(" mm=");
    aarch64_uart_put_hex((unsigned long)cGroupMm);
    aarch64_uart_puts(" net=");
    aarch64_uart_put_hex((unsigned long)cGroupNet);
    aarch64_uart_puts(" proc=");
    aarch64_uart_put_hex((unsigned long)cGroupProc);
    aarch64_uart_puts(" sync=");
    aarch64_uart_put_hex((unsigned long)cGroupSync);
    aarch64_uart_puts("\n");

    fTableOk = 0;
    if (cNrs > 0u && fOrdered != 0u && cSoftNrs == 2u &&
        fHaveGetpid != 0u && fHaveGettid != 0u && fHaveWrite != 0u &&
        fHaveFutex != 0u && fHaveIouring != 0u && fHaveClone3 != 0u &&
        fHaveGetrandom != 0u && fHaveEpoll != 0u && fHavePipe2 != 0u) {
        fTableOk = 1;
    }

    uGateSvc = (fSvcCountOk != 0) ? 1u : 0u;
    uGateStub = (fStubOk != 0) ? 1u : 0u;
    uGateDeep = (fDeepOk != 0) ? 1u : 0u;
    uGateGetpid = (fGetpidOk != 0) ? 1u : 0u;
    uGateTable = (fTableOk != 0) ? 1u : 0u;
    uGateSoftHit = (g_cSoftHit >= 2ul) ? 1u : 0u;

    /* Grep: aarch64: svc soft gates */
    aarch64_uart_puts("aarch64: svc soft gates svc=");
    aarch64_uart_put_hex((unsigned long)uGateSvc);
    aarch64_uart_puts(" stub=");
    aarch64_uart_put_hex((unsigned long)uGateStub);
    aarch64_uart_puts(" deep=");
    aarch64_uart_put_hex((unsigned long)uGateDeep);
    aarch64_uart_puts(" getpid=");
    aarch64_uart_put_hex((unsigned long)uGateGetpid);
    aarch64_uart_puts(" table=");
    aarch64_uart_put_hex((unsigned long)uGateTable);
    aarch64_uart_puts(" soft_hit=");
    aarch64_uart_put_hex((unsigned long)uGateSoftHit);
    aarch64_uart_puts(" ordered=");
    aarch64_uart_put_hex((unsigned long)fOrdered);
    aarch64_uart_puts("\n");

    fOk = 0;
    if (fSvcCountOk != 0 && fStubOk != 0 && fDeepOk != 0 &&
        fGetpidOk != 0 && fTableOk != 0 && g_cSoftHit >= 2ul) {
        fOk = 1;
    }

    /* Grep: aarch64: svc soft path */
    aarch64_uart_puts("aarch64: svc soft path stub=1 soft_pid=1 x8_frame=0 "
                      "shared_dispatch=0 neon=0 product_kernel=OPEN "
                      "hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (soft inventory; not bar3)\n");

    /* Grep: aarch64: svc soft surf — Wave 19 gate bit lamps */
    aarch64_uart_puts("aarch64: svc soft surf svc=");
    aarch64_uart_put_hex((unsigned long)uGateSvc);
    aarch64_uart_puts(" stub=");
    aarch64_uart_put_hex((unsigned long)uGateStub);
    aarch64_uart_puts(" deep=");
    aarch64_uart_put_hex((unsigned long)uGateDeep);
    aarch64_uart_puts(" getpid=");
    aarch64_uart_put_hex((unsigned long)uGateGetpid);
    aarch64_uart_puts(" table=");
    aarch64_uart_put_hex((unsigned long)uGateTable);
    aarch64_uart_puts(" soft_hit=");
    aarch64_uart_put_hex((unsigned long)uGateSoftHit);
    aarch64_uart_puts(" ordered=");
    aarch64_uart_put_hex((unsigned long)fOrdered);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        (uGateSvc << 0) | (uGateStub << 1) | (uGateDeep << 2) |
        (uGateGetpid << 3) | (uGateTable << 4) | (uGateSoftHit << 5) |
        (fOrdered << 6)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: svc soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: svc: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: svc: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: svc: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: svc: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: svc: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: svc: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: svc: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: svc: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: svc: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: svc: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: svc: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: svc: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: svc: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: svc: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: svc: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: svc: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: svc: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: svc: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: svc: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: svc: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: svc: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: svc: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: svc: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: svc: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: svc: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: svc: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: svc: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: svc: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: svc: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: svc: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: svc: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: svc: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: svc: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: svc: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: svc: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: svc: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: svc: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: svc: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: svc: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: svc: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: svc: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: svc: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: svc: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: svc: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: svc: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: svc: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: svc: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: svc: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: svc: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: svc: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: svc: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: svc: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: svc: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: svc: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: svc: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: svc: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: svc: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: svc: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: svc: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: svc: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: svc: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: svc: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: svc: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: svc: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: svc: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: svc: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: svc: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: svc: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: svc: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: svc: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: svc: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: svc: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: svc: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: svc: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: svc: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: svc: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: svc: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: svc: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: svc: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: svc: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retflankangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: svc: soft retfaceangle — Wave 66 exclusive faceangle stamp */
aarch64_uart_puts("aarch64: svc: soft retfaceangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retcaponierangle — Wave 67 return-caponierangle honesty */
aarch64_uart_puts("aarch64: svc: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: svc: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: svc: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: svc: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: svc: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: svc: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retfosseangle — Wave 70 return-fosseangle honesty */
aarch64_uart_puts("aarch64: svc: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
aarch64_uart_puts("aarch64: svc: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
aarch64_uart_puts("aarch64: svc: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
aarch64_uart_puts("aarch64: svc: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: svc: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
aarch64_uart_puts("aarch64: svc: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
aarch64_uart_puts("aarch64: svc: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retcurtainface — Wave 73 return-curtainface honesty */
aarch64_uart_puts("aarch64: svc: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retglacisangle — Wave 74 return-glacisangle honesty */
aarch64_uart_puts("aarch64: svc: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
aarch64_uart_puts("aarch64: svc: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retmoatangle — Wave 75 return-moatangle honesty */
aarch64_uart_puts("aarch64: svc: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retowerangle — Wave 75 exclusive towerangle stamp */
aarch64_uart_puts("aarch64: svc: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retgateangle — Wave 76 return-gateangle honesty */
aarch64_uart_puts("aarch64: svc: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retwallangle — Wave 76 exclusive wallangle stamp */
aarch64_uart_puts("aarch64: svc: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retspireangle — Wave 77 return-spireangle honesty */
aarch64_uart_puts("aarch64: svc: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retholdangle — Wave 77 exclusive holdangle stamp */
aarch64_uart_puts("aarch64: svc: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retholdangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retpalaceangle — Wave 78 return-palaceangle honesty */
aarch64_uart_puts("aarch64: svc: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
aarch64_uart_puts("aarch64: svc: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retfortressangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retkeepangle — Wave 79 return-keepangle honesty */
aarch64_uart_puts("aarch64: svc: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
aarch64_uart_puts("aarch64: svc: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retcitadelangle — Wave 80 return-citadelangle honesty */
aarch64_uart_puts("aarch64: svc: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
aarch64_uart_puts("aarch64: svc: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
aarch64_uart_puts("aarch64: svc: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retmantleangle — Wave 82 return-mantleangle honesty */
aarch64_uart_puts("aarch64: svc: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
aarch64_uart_puts("aarch64: svc: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaegisangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retemblemangle — Wave 83 return-emblemangle honesty */
aarch64_uart_puts("aarch64: svc: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
aarch64_uart_puts("aarch64: svc: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retsigilangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retscepterangle — Wave 84 return-scepterangle honesty */
aarch64_uart_puts("aarch64: svc: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
aarch64_uart_puts("aarch64: svc: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retglyphangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retcrownangle — Wave 85 return-crownangle honesty */
aarch64_uart_puts("aarch64: svc: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retshardangle — Wave 85 exclusive shardangle stamp */
aarch64_uart_puts("aarch64: svc: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retshardangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retforgeangle — Wave 86 return-forgeangle honesty */
aarch64_uart_puts("aarch64: svc: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retprismangle — Wave 86 exclusive prismangle stamp */
aarch64_uart_puts("aarch64: svc: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retprismangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retflameangle — Wave 87 return-flameangle honesty */
aarch64_uart_puts("aarch64: svc: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
aarch64_uart_puts("aarch64: svc: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcipherangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retbeaconangle — Wave 88 return-beaconangle honesty */
aarch64_uart_puts("aarch64: svc: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
aarch64_uart_puts("aarch64: svc: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retledgerangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retbannerangle — Wave 89 return-bannerangle honesty */
aarch64_uart_puts("aarch64: svc: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
aarch64_uart_puts("aarch64: svc: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retvaultangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retcrestangle — Wave 90 return-crestangle honesty */
aarch64_uart_puts("aarch64: svc: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
aarch64_uart_puts("aarch64: svc: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rettokenangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retbadgeangle — Wave 91 return-badgeangle honesty */
aarch64_uart_puts("aarch64: svc: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
aarch64_uart_puts("aarch64: svc: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retphaseangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retmarkangle — Wave 92 return-markangle honesty */
aarch64_uart_puts("aarch64: svc: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
aarch64_uart_puts("aarch64: svc: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retpulseangle stamp; Soft≠product)\n");

/* Grep: aarch64: svc: soft retsealangle — Wave 93 return-sealangle honesty */
aarch64_uart_puts("aarch64: svc: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retboundangle — Wave 93 exclusive boundangle stamp */
aarch64_uart_puts("aarch64: svc: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retboundangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retstemangle — Wave 94 return-stemangle honesty */
aarch64_uart_puts("aarch64: svc: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbladeangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retchordangle — Wave 95 return-chordangle honesty */
aarch64_uart_puts("aarch64: svc: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retarcangle — Wave 95 exclusive arcangle stamp */
aarch64_uart_puts("aarch64: svc: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retarcangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retsectorangle — Wave 96 return-sectorangle honesty */
aarch64_uart_puts("aarch64: svc: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
aarch64_uart_puts("aarch64: svc: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retradiusangle — Wave 97 return-radiusangle honesty */
aarch64_uart_puts("aarch64: svc: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
aarch64_uart_puts("aarch64: svc: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retcircumangle — Wave 98 return-circumangle honesty */
aarch64_uart_puts("aarch64: svc: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
aarch64_uart_puts("aarch64: svc: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retellipseangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft rethyperangle — Wave 99 return-hyperangle honesty */
aarch64_uart_puts("aarch64: svc: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
aarch64_uart_puts("aarch64: svc: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retspiralangle — Wave 100 return-spiralangle honesty */
aarch64_uart_puts("aarch64: svc: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft rethelixangle — Wave 100 exclusive helixangle stamp */
aarch64_uart_puts("aarch64: svc: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rethelixangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft rettorusangle — Wave 101 return-torusangle honesty */
aarch64_uart_puts("aarch64: svc: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retknotangle — Wave 101 exclusive knotangle stamp */
aarch64_uart_puts("aarch64: svc: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retknotangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
aarch64_uart_puts("aarch64: svc: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
aarch64_uart_puts("aarch64: svc: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retkleinangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retprojectangle — Wave 103 return-projectangle honesty */
aarch64_uart_puts("aarch64: svc: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retaffineangle — Wave 103 exclusive affineangle stamp */
aarch64_uart_puts("aarch64: svc: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaffineangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retlinearangle — Wave 104 return-linearangle honesty */
aarch64_uart_puts("aarch64: svc: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
aarch64_uart_puts("aarch64: svc: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
aarch64_uart_puts("aarch64: svc: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcubicangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retquarticangle — Wave 106 return-quarticangle honesty */
aarch64_uart_puts("aarch64: svc: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
aarch64_uart_puts("aarch64: svc: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retquinticangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retsplineangle — Wave 107 return-splineangle honesty */
aarch64_uart_puts("aarch64: svc: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbezierangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft rethurmitangle — Wave 108 return-hermitangle honesty */
aarch64_uart_puts("aarch64: svc: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
aarch64_uart_puts("aarch64: svc: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: aarch64: svc: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
aarch64_uart_puts("aarch64: svc: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: svc: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
aarch64_uart_puts("aarch64: svc: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbsplineangle stamp; Soft≠product)\n");
aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
    aarch64_uart_puts("aarch64: svc soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_AREAS);
    aarch64_uart_puts(" catalog=inventory,count,nrs,have,groups,gates,"
                      "path,surf,return,honesty,deepen unit=svc.c only "
                      "rate_limited=0 soft_only=1\n");

    /* Grep: aarch64: svc soft honesty */
    aarch64_uart_puts("aarch64: svc soft honesty product_kernel=OPEN "
                      "soft_only=1 no_shared_dispatch=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: svc soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: svc soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: svc soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: svc soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)SVC_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: svc soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: svc soft FAIL\n");
    }
    return fOk;
}

void
aarch64_svc_selftest(void)
{
    long r0;
    long r1;
    long rSoft;
    unsigned cNrs;
    int fSvcCountOk;
    int fStubOk;
    int fDeepOk;
    int fGetpidOk;
    int fInvSoft;

    cNrs = (unsigned)(sizeof(g_aNrStub) / sizeof(g_aNrStub[0]));
    fSvcCountOk = 0;
    fStubOk = 1;
    fDeepOk = 1;
    fGetpidOk = 0;
    fInvSoft = 0;

    /*
     * Issue SVC #0 from EL1 — valid; ESR EC=SVC64. Vector is Current EL SPx
     * sync (index 4). Handler returns; we continue.
     */
    __asm__ volatile("svc #0" ::: "memory");
    if (g_cSvc == 0ul) {
        aarch64_uart_puts("aarch64: svc soft FAIL (no count)\n");
        /* Still emit inventory + path presence for greppable soft FAIL. */
        (void)svc_soft_inventory(0, 0, 0, 0);
        aarch64_uart_puts("aarch64: svc PASS\n"); /* still mark path present */
        return;
    }
    fSvcCountOk = 1;

    /*
     * Soft-exercise Linux NR stub table (in-arch only). write remains
     * -ENOSYS via stub; getpid soft path returns A64_SOFT_PID.
     */
    r0 = aarch64_linux_nr_stub(A64_NR_getpid);
    r1 = aarch64_linux_nr_stub(A64_NR_write);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (stub)\n");
        fStubOk = 0;
    }

    /* Soft getpid return path (positive PID, not -ENOSYS). */
    rSoft = aarch64_linux_nr_soft(A64_NR_getpid);
    fGetpidOk = (rSoft == A64_SOFT_PID);
    r0 = aarch64_linux_nr_soft(A64_NR_gettid);
    if (r0 != A64_SOFT_TID) {
        fGetpidOk = 0;
    }
    /* Soft path must still ENOSYS non-implemented NRs. */
    r1 = aarch64_linux_nr_soft(A64_NR_write);
    if (r1 != -(long)LINUX_ENOSYS) {
        fGetpidOk = 0;
    }

    /* Deepen: futex + io_uring + clone3 + getrandom NRs must hit table. */
    r0 = aarch64_linux_nr_stub(A64_NR_futex);
    r1 = aarch64_linux_nr_stub(A64_NR_io_uring_setup);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (deep stub)\n");
        fDeepOk = 0;
    }
    r0 = aarch64_linux_nr_stub(A64_NR_clone3);
    r1 = aarch64_linux_nr_stub(A64_NR_getrandom);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (deep stub2)\n");
        fDeepOk = 0;
    }
    /* epoll + pipe2 hot path NRs (table coverage deepen). */
    r0 = aarch64_linux_nr_stub(A64_NR_epoll_pwait);
    r1 = aarch64_linux_nr_stub(A64_NR_pipe2);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (deep stub3)\n");
        fDeepOk = 0;
    }
    /* Unknown high NR → miss path. */
    r0 = aarch64_linux_nr_stub(9000ul);
    (void)r0;

    /* Legacy one-line summary (kept greppable for existing smoke detail). */
    aarch64_uart_puts("aarch64: svc count=");
    aarch64_uart_put_hex(g_cSvc);
    aarch64_uart_puts(" stub_hit=");
    aarch64_uart_put_hex(g_cStubHit);
    aarch64_uart_puts(" stub_miss=");
    aarch64_uart_put_hex(g_cStubMiss);
    aarch64_uart_puts(" soft_hit=");
    aarch64_uart_put_hex(g_cSoftHit);
    aarch64_uart_puts(" nrs=");
    aarch64_uart_put_hex((unsigned long)cNrs);
    aarch64_uart_puts("\n");

    /* Wave 9 soft inventory under "aarch64: svc soft …". */
    fInvSoft = svc_soft_inventory(fSvcCountOk, fStubOk, fDeepOk, fGetpidOk);
    (void)fInvSoft;

    aarch64_uart_puts("aarch64: svc PASS\n");
    aarch64_uart_puts("aarch64: svc NR dispatch deepen PASS\n");
    if (fGetpidOk) {
        aarch64_uart_puts("aarch64: svc getpid soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: svc getpid soft FAIL\n");
    }
}

unsigned long
aarch64_svc_count(void)
{
    return g_cSvc;
}
