/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Interrupt Descriptor Table (64-bit) + soft gate observability.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * Design: DESIGN_SPEC_COMPLETE G-IDT-*.
 * Product path installs a shared 256-entry IDT used by BSP and all APs:
 *   - Vectors 0..31: CPU exceptions (base layout from idt_init).
 *   - Vector 128 (0x80): int 0x80 compat / PE32 smoke (DPL3).
 *   - Dynamic gates via idt_set_gate for PIC remap, MSI-X, IPI vectors.
 *
 * Soft IDT gate observability: exception / int80 / dynamic gate install
 * counters, last-gate snapshot, type/DPL decode, greppable soft logs.
 * Soft never hard-gates product; wrap-OK counters only.
 *
 * Base layout contract (soft verify)
 * ----------------------------------
 *   - idt_ready after idt_init
 *   - vec 0..31 present, selector = kernel CS (GJ_GDT_KERNEL_CS)
 *   - vec 3,4 DPL3 trap-style (type 0xEF) — BP/OF soft user enter
 *   - other exceptions DPL0 interrupt (type 0x8E)
 *   - vec 128 present DPL3 interrupt (type 0xEE)
 *
 * AP load order: gdt_load_ap() then idt_load_ap() before unmasking IRQs
 * (gates encode kernel CS; wrong GDT → #GP on first IRQ).
 *
 * Cross-module: gj/trap.h (frame + dispatch), gj/gdt.h (selectors),
 *               gj/apic.h (IPI vectors 49/50), arch isr stubs.
 *
 * greppable: idt: gate soft
 * greppable: GJ_IDT_SOFT_TYPE_ GJ_IDT_SOFT_ATTR_
 * greppable: idt_init idt_set_gate idt_gate_soft_log
 * greppable: G-IDT int80 0x8E 0xEE
 */
#pragma once

#include <gj/types.h>

/**
 * Build base IDT (exceptions + int 0x80) and lidt on the BSP.
 * Soft-notes install counters; stamps base-ready for soft verify.
 */
void idt_init(void);

/** Non-zero after idt_init; required before untrusted ring-3. */
int idt_ready(void);

/**
 * lidt the shared IDT (call on each AP after gdt_load_ap).
 * Does not rebuild gates — only reloads IDTR to the BSP-built table.
 */
void idt_load_ap(void);

/**
 * Install a 64-bit interrupt gate (type typically 0x8E).
 * Used for IRQ vectors after PIC remap, MSI-X, and IPI handlers.
 * Soft-rejects vec >= 256 or NULL handler (bumps reject counter).
 */
void idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type);

/* ------------------------------------------------------------------ */
/* Soft IDT gate observability (boot telemetry — not hot-path)        */
/* ------------------------------------------------------------------ */

/** Gate type/DPL decode aids (Intel IDT type attribute byte). */
#define GJ_IDT_SOFT_TYPE_INT_GATE   0x0Eu /* 64-bit interrupt gate */
#define GJ_IDT_SOFT_TYPE_TRAP_GATE  0x0Fu /* 64-bit trap gate */
#define GJ_IDT_SOFT_ATTR_P          0x80u
#define GJ_IDT_SOFT_ATTR_DPL_SHIFT  5
#define GJ_IDT_SOFT_ATTR_DPL_MASK   0x60u

/**
 * Soft snapshot of last installed gate + inventory lamps.
 * Present/DPL/type tallies are refreshed by idt_gate_soft_inventory.
 */
struct gj_idt_gate_soft {
    u32 u32LastVec;     /* last vector written */
    u64 u64LastOff;     /* last handler offset */
    u16 u16LastSel;     /* last selector (expect kernel CS) */
    u8  u8LastType;     /* last type attribute byte */
    u8  u8LastIst;      /* last IST index */
    u8  u8LastDpl;      /* soft decode DPL 0..3 */
    u8  u8LastTrap;     /* 1 if trap-style type nibble 0xF */
    u8  u8BaseReady;    /* 1 after idt_init base layout (32 + int80) */
    u8  u8VerifyOk;     /* 1 if last soft verify passed */
    u16 u16Pad;
    u32 u32Present;     /* soft count of present gates (P bit) */
    u32 u32Dpl0;        /* present DPL0 gates */
    u32 u32Dpl3;        /* present DPL3 gates */
    u32 u32IntStyle;    /* present interrupt-gate type */
    u32 u32TrapStyle;   /* present trap-gate type */
};

/** idt_init soft notes. */
u32  idt_gate_soft_inits(void);
/** idt_load_ap soft notes. */
u32  idt_gate_soft_ap_loads(void);
/** Total successful idt_set / idt_set_gate installs (incl. base). */
u32  idt_gate_soft_installs(void);
/** Installs into vectors 0..31 (exception). */
u32  idt_gate_soft_exception(void);
/** Installs of vector 128 (int 0x80). */
u32  idt_gate_soft_int80(void);
/** Installs of other vectors (IRQ / MSI-X / …). */
u32  idt_gate_soft_irq(void);
/** Rejected installs (vec≥256 or NULL handler). */
u32  idt_gate_soft_reject(void);
/** Soft verify PASS / FAIL. */
u32  idt_gate_soft_verify_ok(void);
u32  idt_gate_soft_verify_bad(void);

/** Last installed vector / type / offset (0 until any install). */
u32  idt_gate_soft_last_vec(void);
u8   idt_gate_soft_last_type(void);
u64  idt_gate_soft_last_off(void);

/**
 * Soft present-gate inventory (walk live IDT; pure read).
 * Updates soft present/DPL/type tallies in the soft snapshot.
 * Returns number of present gates.
 */
u32  idt_gate_soft_inventory(void);

/**
 * Soft verify base layout:
 *   - IDT ready
 *   - vec 0..31 present, sel=kernel CS
 *   - vec 3,4 DPL3 trap-style (0xEF)
 *   - other exceptions DPL0 interrupt (0x8E)
 *   - vec 128 present DPL3 interrupt (0xEE)
 * Bumps soft verify counters. Returns 1 on PASS, 0 on FAIL.
 */
int  idt_gate_soft_verify(void);

/**
 * Fill *pOut with last soft snapshot (zeros if never init).
 * Returns 1 if base soft-ready, 0 otherwise.
 */
int  idt_gate_soft_info_get(struct gj_idt_gate_soft *pOut);

/**
 * Greppable soft summary:
 *   idt: gate soft inits=… ap=… install=… exc=… int80=… irq=… reject=…
 *   idt: gate soft present=… dpl0=… dpl3=… int=… trap=… verify_ok=… verify_bad=…
 *   idt: gate soft last vec=… type=0x… dpl=… trap=… off=0x… sel=0x…
 *   idt: gate soft verify PASS|FAIL|idle
 * greppable: idt: gate soft
 */
void idt_gate_soft_log(void);
