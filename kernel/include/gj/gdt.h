/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
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
u64  tss_get_rsp0(void);
/**
 * Dedicated IRQ/exception stack from gdt_init (g_aRsp0Stack).
 * Never point TSS.RSP0 at a thread SYSCALL kstack — that collides with a
 * parked thr's mid-syscall frames when another thr takes an IRQ.
 */
u64  tss_irq_rsp0(void);
/** Restore TSS.RSP0 to the dedicated IRQ stack. */
void tss_use_irq_rsp0(void);
