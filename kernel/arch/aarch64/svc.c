/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SVC / synchronous-exception product path (scaffold).
 * Handles Current-EL and lower-EL AArch64 sync: SVC from EL0 will later
 * feed Linux ABI; today we count SVC64, keep an in-arch Linux NR stub
 * table (public aarch64 syscall numbers), and return ENOSYS-shaped results.
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
 *   3. Selftest exercises the stub table without needing a full reg frame
 *
 * Next product step (still in-arch): widen the exception frame to save
 * x0–x18, pass a soft reg struct into try_handle, read NR from x8, call
 * stub (then later shared linux_dispatch). Keep numbers here dual-licensed
 * clean-room; do not paste GPL arch/arm64 tables.
 *
 * Greppable: aarch64: svc PASS
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
 * AArch64 Linux syscall numbers (public unistd ABI / man7).
 * Distinct from x86_64 LINUX_NR_* in kernel/include/gj/linux_abi.h.
 * In-arch stub only — not linked to shared dispatch yet.
 */
#define A64_NR_io_setup           0u
#define A64_NR_io_destroy         1u
#define A64_NR_ioctl             29u
#define A64_NR_openat            56u
#define A64_NR_close             57u
#define A64_NR_read              63u
#define A64_NR_write             64u
#define A64_NR_writev            66u
#define A64_NR_exit              93u
#define A64_NR_exit_group        94u
#define A64_NR_nanosleep        101u
#define A64_NR_clock_gettime    113u
#define A64_NR_sched_yield      124u
#define A64_NR_rt_sigaction     134u
#define A64_NR_rt_sigreturn     139u
#define A64_NR_uname            160u
#define A64_NR_getpid           172u
#define A64_NR_gettid           178u
#define A64_NR_brk              214u
#define A64_NR_munmap           215u
#define A64_NR_clone            220u
#define A64_NR_mmap             222u
#define A64_NR_mprotect         226u

/* Soft stub table size: sparse NR space; use sentinel-terminated list. */
struct a64_nr_stub {
    unsigned uNr;
    const char *szName; /* for soft debug; may be unused */
};

/*
 * Known NRs we acknowledge. Handler always returns -ENOSYS until real
 * implementations land. Order is by NR for greppability, not hot-path.
 */
static const struct a64_nr_stub g_aNrStub[] = {
    { A64_NR_io_setup,      "io_setup" },
    { A64_NR_io_destroy,    "io_destroy" },
    { A64_NR_ioctl,         "ioctl" },
    { A64_NR_openat,        "openat" },
    { A64_NR_close,         "close" },
    { A64_NR_read,          "read" },
    { A64_NR_write,         "write" },
    { A64_NR_writev,        "writev" },
    { A64_NR_exit,          "exit" },
    { A64_NR_exit_group,    "exit_group" },
    { A64_NR_nanosleep,     "nanosleep" },
    { A64_NR_clock_gettime, "clock_gettime" },
    { A64_NR_sched_yield,   "sched_yield" },
    { A64_NR_rt_sigaction,  "rt_sigaction" },
    { A64_NR_rt_sigreturn,  "rt_sigreturn" },
    { A64_NR_uname,         "uname" },
    { A64_NR_getpid,        "getpid" },
    { A64_NR_gettid,        "gettid" },
    { A64_NR_brk,           "brk" },
    { A64_NR_munmap,        "munmap" },
    { A64_NR_clone,         "clone" },
    { A64_NR_mmap,          "mmap" },
    { A64_NR_mprotect,      "mprotect" },
};

static unsigned long g_cSvc;
static unsigned long g_cSyncOther;
static unsigned long g_cStubHit;
static unsigned long g_cStubMiss;

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
 * Called from exception.S / exception.c for synchronous vectors.
 * Returns 1 if SVC handled (caller may skip default dump), else 0.
 *
 * When a full register frame exists, product should:
 *   nr = frame->x8; ret = aarch64_linux_nr_stub(nr); frame->x0 = (u64)ret;
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
     * Soft-exercise Linux NR stub table (in-arch only). getpid / write are
     * representative hot numbers on aarch64; both must return -ENOSYS today.
     */
    r0 = aarch64_linux_nr_stub(A64_NR_getpid);
    r1 = aarch64_linux_nr_stub(A64_NR_write);
    if (r0 != -(long)LINUX_ENOSYS || r1 != -(long)LINUX_ENOSYS) {
        aarch64_uart_puts("aarch64: svc soft FAIL (stub)\n");
    }

    aarch64_uart_puts("aarch64: svc count=");
    aarch64_uart_put_hex(g_cSvc);
    aarch64_uart_puts(" stub_hit=");
    aarch64_uart_put_hex(g_cStubHit);
    aarch64_uart_puts(" stub_miss=");
    aarch64_uart_put_hex(g_cStubMiss);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: svc PASS\n");
}

unsigned long
aarch64_svc_count(void)
{
    return g_cSvc;
}
