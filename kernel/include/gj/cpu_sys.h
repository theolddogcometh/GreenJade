/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL MSRs + ring3 helpers (Option C entry).
 *
 * Soft SYSCALL MSR observability: boot/bring-up telemetry for
 * STAR/LSTAR/CSTAR/SFMASK/EFER.SCE — counters, last programmed snapshot,
 * readback verify, greppable logs. Not hot-path locks.
 */
#pragma once

#include <gj/types.h>

void cpu_syscall_init(void);

/** True after MSRs programmed for SYSCALL/SYSRET. */
int cpu_syscall_ready(void);

/**
 * Enter ring 3 at u64Entry with stack u64Stack (must be mapped user-accessible).
 * Does not return on success. Stub returns if not ready / invalid.
 */
void cpu_enter_user(u64 u64Entry, u64 u64Stack) __attribute__((noreturn));

/**
 * Enter 32-bit compat ring-3 via iretq (CS32 L=0 D=1, SS=user DS).
 * Does not return on success. Used for PE32/WoW64 hardware enter smoke.
 */
void cpu_enter_user32(u64 u64Entry, u64 u64Stack) __attribute__((noreturn));

/* ------------------------------------------------------------------ */
/* Soft SYSCALL MSR observability (boot telemetry — not hot-path)     */
/* ------------------------------------------------------------------ */

/** Canonical STAR user base for this GDT (CS32=base+0|RPL3, DS=+8, CS64=+16). */
#define GJ_CPU_SYSCALL_STAR_USER_BASE  0x18u
/** Expected kernel CS in STAR[47:32]. */
#define GJ_CPU_SYSCALL_STAR_KERNEL_CS  0x08u

/**
 * Soft snapshot of last programmed SYSCALL MSRs (post-init values).
 * Zeroed until cpu_syscall_init; soft-only (does not re-rdmsr).
 */
struct gj_cpu_syscall_soft {
    u64 u64Star;       /* IA32_STAR */
    u64 u64Lstar;      /* IA32_LSTAR */
    u64 u64Cstar;      /* IA32_CSTAR */
    u64 u64Sfmask;     /* IA32_FMASK / SFMASK */
    u64 u64Efer;       /* IA32_EFER after SCE|NXE */
    u16 u16StarKernCs; /* STAR[47:32] decode */
    u16 u16StarUserBase; /* STAR[63:48] decode */
    u8  u8Sce;         /* EFER.SCE soft bit */
    u8  u8Nxe;         /* EFER.NXE soft bit */
    u8  u8VerifyOk;    /* 1 if last soft verify passed */
    u8  u8Pad;
};

/** Times cpu_syscall_init programmed the MSRs. */
u32  cpu_syscall_soft_inits(void);
/** Soft verify PASS count (readback matches program + SCE). */
u32  cpu_syscall_soft_verify_ok(void);
/** Soft verify FAIL count. */
u32  cpu_syscall_soft_verify_bad(void);
/** Soft enter_user attempts (long mode sysretq path). */
u32  cpu_syscall_soft_enter64(void);
/** Soft enter_user32 attempts (compat iretq path). */
u32  cpu_syscall_soft_enter32(void);
/** Soft enter_user invalid rejections. */
u32  cpu_syscall_soft_enter_bad(void);

/** Last soft-snapshotted STAR / LSTAR / SFMASK / EFER (0 until init). */
u64  cpu_syscall_soft_star(void);
u64  cpu_syscall_soft_lstar(void);
u64  cpu_syscall_soft_sfmask(void);
u64  cpu_syscall_soft_efer(void);

/**
 * Fill *pOut with last soft snapshot (zeros if never init).
 * Returns 1 if soft snapshot is live (ready), 0 otherwise.
 */
int  cpu_syscall_soft_info_get(struct gj_cpu_syscall_soft *pOut);

/**
 * Soft re-verify: rdmsr STAR/LSTAR/CSTAR/SFMASK/EFER and compare to
 * last programmed soft snapshot + SCE required. Bumps ok/bad counters.
 * Returns 1 on PASS, 0 on FAIL / not ready.
 */
int  cpu_syscall_soft_verify(void);

/**
 * Greppable soft summary:
 *   cpu: syscall soft inits=… verify_ok=… verify_bad=… enter64=… enter32=… bad=…
 *   cpu: syscall soft STAR=0x… LSTAR=0x… SFMASK=0x… EFER=0x… SCE=… NXE=…
 *   cpu: syscall soft decode kern_cs=0x… user_base=0x… (CS64=base+16)
 *   cpu: syscall soft verify PASS|FAIL|idle
 */
void cpu_syscall_soft_log(void);
