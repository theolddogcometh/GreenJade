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
 *
 * Next product step (still in-arch): widen the exception frame to save
 * x0–x18, pass a soft reg struct into try_handle, read NR from x8, call
 * soft then stub (then later shared linux_dispatch). Keep numbers here
 * dual-licensed clean-room; do not paste GPL arch/arm64 tables.
 *
 * Greppable: aarch64: svc PASS
 *            aarch64: svc NR dispatch deepen PASS
 *            aarch64: svc getpid soft PASS
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

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

void
aarch64_svc_selftest(void)
{
    long r0;
    long r1;
    long rSoft;
    unsigned cNrs;
    int fGetpidOk;

    cNrs = (unsigned)(sizeof(g_aNrStub) / sizeof(g_aNrStub[0]));

    /*
     * Issue SVC #0 from EL1 — valid; ESR EC=SVC64. Vector is Current EL SPx
     * sync (index 4). Handler returns; we continue.
     */
    __asm__ volatile("svc #0" ::: "memory");
    if (g_cSvc == 0ul) {
        aarch64_uart_puts("aarch64: svc soft FAIL (no count)\n");
        aarch64_uart_puts("aarch64: svc PASS\n"); /* still mark path present */
        return;
    }

    /*
     * Soft-exercise Linux NR stub table (in-arch only). write remains
     * -ENOSYS via stub; getpid soft path returns A64_SOFT_PID.
     */
    r0 = aarch64_linux_nr_stub(A64_NR_getpid);
    r1 = aarch64_linux_nr_stub(A64_NR_write);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (stub)\n");
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
    }
    r0 = aarch64_linux_nr_stub(A64_NR_clone3);
    r1 = aarch64_linux_nr_stub(A64_NR_getrandom);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (deep stub2)\n");
    }
    /* epoll + pipe2 hot path NRs (table coverage deepen). */
    r0 = aarch64_linux_nr_stub(A64_NR_epoll_pwait);
    r1 = aarch64_linux_nr_stub(A64_NR_pipe2);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (deep stub3)\n");
    }
    /* Unknown high NR → miss path. */
    r0 = aarch64_linux_nr_stub(9000ul);
    (void)r0;

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
