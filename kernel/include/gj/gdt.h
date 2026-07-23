/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Runtime GDT + TSS (long mode) and soft user-segment observability.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * Product x86_64 segmentation for kernel and ring-3 (P-ABI / Option C):
 *   - Kernel CS/DS for IDT gates and SYSCALL kernel entry.
 *   - User CS32 / DS / CS64 layout matching IA32_STAR user base 0x18.
 *   - TSS with RSP0 (IRQ/exception ring-3 stack) and soft IST snapshot.
 *
 * Soft user-segment observability: CS32 / DS / CS64 descriptor inventory,
 * LAR probe counters, greppable soft logs. Soft never hard-gates boot.
 *
 * STAR / selector layout (canonical for this GDT)
 * -----------------------------------------------
 *   Index 1  0x08  kernel CS
 *   Index 2  0x10  kernel DS
 *   Index 3  0x18  user CS32 (compat / WoW64) — STAR user base
 *   Index 4  0x20  user DS   → selector | RPL3 = 0x23
 *   Index 5  0x28  user CS64 → selector | RPL3 = 0x2B  (SYSRETQ target)
 *   Index 6  0x30  TSS
 *
 * TSS.RSP0 rule
 * -------------
 * Never point TSS.RSP0 at a thread SYSCALL kstack — that collides with a
 * parked thr's mid-syscall frames when another thr takes an IRQ.
 * gdt_init installs a dedicated IRQ stack (tss_irq_rsp0); restore via
 * tss_use_irq_rsp0 after any temporary RSP0 retarget.
 *
 * AP load order: gdt_load_ap() before idt_load_ap() — IRQ gates use kernel CS.
 * Shared TSS is BSP-only (AP LTR is not performed by gdt_load_ap).
 *
 * greppable: gdt: user soft
 * greppable: GJ_GDT_USER_ CS32 LAR TSS.RSP0
 * greppable: gdt_init gdt_load_ap gdt_user_soft_log
 * greppable: STAR 0x18 WoW64
 */
#pragma once

#include <gj/types.h>

#define GJ_GDT_KERNEL_CS 0x08u
#define GJ_GDT_KERNEL_DS 0x10u
/* STAR user base 0x18: +0 = CS32, +8 = DS, +16 = CS64 (all | RPL3) */
#define GJ_GDT_USER_CS32 0x1Bu /* index 3 | RPL3 — 32-bit user code (WoW64) */
#define GJ_GDT_USER_DS   0x23u /* index 4 | RPL3 — user data */
#define GJ_GDT_USER_CS   0x2Bu /* index 5 | RPL3 — long mode user code */
#define GJ_GDT_TSS_SEL   0x30u

/**
 * Build and load the product GDT + TSS on the BSP.
 * Soft-notes user segment bytes and readiness lamps; stamps soft init.
 */
void gdt_init(void);

/** Non-zero if GDT has a usable 32-bit user code descriptor (WoW64 ready). */
int gdt_user_cs32_ready(void);
/** Selector value for 32-bit user code (RPL3). */
u16 gdt_user_cs32_sel(void);
/**
 * Validate CS32 is true 32-bit compat (L=0, D=1, DPL3, present code).
 * Returns 1 if descriptor is correct for iretq into 32-bit user mode.
 */
int gdt_user_cs32_is_compat(void);

/**
 * CPU LAR (load access rights) probe of CS32 selector.
 * Returns 1 if LAR succeeds and rights show P|DPL3|code (hardware-visible).
 * Fills *pOutAr with LAR result when non-NULL.
 */
int gdt_user_cs32_lar_ok(u32 *pOutAr);

/**
 * Load kernel GDT + reload CS/DS on an AP (no LTR — shared TSS is BSP-only).
 * Required before AP enables IDT IRQs (gate selectors are kernel CS).
 */
void gdt_load_ap(void);

/** Update TSS.RSP0 (ring-3 interrupt stack). */
void tss_set_rsp0(u64 u64Rsp0);
/** Read current TSS.RSP0. */
u64  tss_get_rsp0(void);
/**
 * Dedicated IRQ/exception stack from gdt_init (g_aRsp0Stack).
 * Never point TSS.RSP0 at a thread SYSCALL kstack — that collides with a
 * parked thr's mid-syscall frames when another thr takes an IRQ.
 */
u64  tss_irq_rsp0(void);
/** Restore TSS.RSP0 to the dedicated IRQ stack. */
void tss_use_irq_rsp0(void);

/* ------------------------------------------------------------------ */
/* Soft GDT user-segment observability (boot telemetry)               */
/* ------------------------------------------------------------------ */

/**
 * Soft snapshot of user segment descriptors + readiness lamps.
 * Access/gran are raw GDT bytes; flags are soft verify results.
 * Zeroed until gdt_init soft note.
 */
struct gj_gdt_user_soft {
    u8  u8Cs32Access;  /* index 3 access byte */
    u8  u8Cs32Gran;    /* index 3 gran byte */
    u8  u8DsAccess;    /* index 4 */
    u8  u8DsGran;
    u8  u8Cs64Access;  /* index 5 */
    u8  u8Cs64Gran;
    u8  u8Cs32Ready;   /* software ready (present DPL3 code D=1) */
    u8  u8Cs32Compat;  /* L=0 D=1 DPL3 code */
    u8  u8Cs64Long;    /* L=1 D=0 DPL3 code (long-mode user) */
    u8  u8DsOk;        /* present DPL3 data */
    u8  u8LarOk;       /* last LAR soft result (0 if never) */
    u8  u8Init;        /* non-zero after gdt_init soft note */
    u16 u16Cs32Sel;    /* GJ_GDT_USER_CS32 */
    u16 u16DsSel;      /* GJ_GDT_USER_DS */
    u16 u16Cs64Sel;    /* GJ_GDT_USER_CS */
    u32 u32LarAr;      /* last LAR access-rights word */
    u64 u64Rsp0;       /* TSS.RSP0 at soft note */
    u64 u64Ist1;       /* TSS.IST1 at soft note */
};

/** gdt_init soft notes (BSP). */
u32  gdt_user_soft_inits(void);
/** gdt_load_ap soft notes. */
u32  gdt_user_soft_ap_loads(void);
/** LAR probe attempts (gdt_user_cs32_lar_ok). */
u32  gdt_user_soft_lar_probes(void);
/** LAR soft PASS count. */
u32  gdt_user_soft_lar_ok(void);
/** LAR soft FAIL count. */
u32  gdt_user_soft_lar_bad(void);
/** Soft full user-segment verify PASS count. */
u32  gdt_user_soft_verify_ok(void);
/** Soft full user-segment verify FAIL count. */
u32  gdt_user_soft_verify_bad(void);

/** Raw GDT access/gran for user CS32 / DS / CS64 (0 until gdt_init). */
u8   gdt_user_soft_cs32_access(void);
u8   gdt_user_soft_cs32_gran(void);
u8   gdt_user_soft_ds_access(void);
u8   gdt_user_soft_cs64_access(void);
u8   gdt_user_soft_cs64_gran(void);

/**
 * Soft: CS64 is long-mode user code (P|DPL3|code, L=1, D=0).
 * Returns 1 if descriptor matches SYSRETQ target.
 */
int  gdt_user_cs64_is_long(void);

/**
 * Soft: user DS is present DPL3 data (SS/DS/ES for ring-3).
 * Returns 1 if descriptor is usable.
 */
int  gdt_user_ds_ok(void);

/**
 * Soft full verify of CS32 compat + CS64 long + DS + (optional LAR).
 * fDoLar: non-zero → also run LAR probe. Bumps soft counters.
 * Returns 1 on PASS, 0 on FAIL / not init.
 */
int  gdt_user_soft_verify(int fDoLar);

/**
 * Fill *pOut with last soft snapshot (zeros if never init).
 * Returns 1 if soft snapshot is live, 0 otherwise.
 */
int  gdt_user_soft_info_get(struct gj_gdt_user_soft *pOut);

/**
 * Greppable soft summary:
 *   gdt: user soft inits=… ap=… lar_probe=… lar_ok=… lar_bad=… verify_ok=… verify_bad=…
 *   gdt: user soft cs32 acc=0x… gran=0x… ready=… compat=… sel=0x…
 *   gdt: user soft ds acc=0x… ok=… sel=0x… cs64 acc=0x… gran=0x… long=… sel=0x…
 *   gdt: user soft tss rsp0=0x… ist1=0x… lar_ar=0x…
 *   gdt: user soft verify PASS|FAIL|idle
 * greppable: gdt: user soft
 */
void gdt_user_soft_log(void);
