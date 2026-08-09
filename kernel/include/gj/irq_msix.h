/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X vector -> Notification bind (hard IRQ + soft pulse paths).
 * Clean-room pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL source.
 * Soft != product (Soft!=product dual license residual - eng lamps only).
 *
 * Delivery model:
 *   IDT gate @ GJ_MSIX_IRQ_VEC (0x41) -> irq_msix_handler ->
 *   notify_pulse on global MSI-X Notification (notify_msix_global) + EOI.
 *   Soft paths inject the same Notification without self-IPI during early
 *   kmain (self-IPI risks #DF before APIC/IDT fully settled).
 *
 * Soft pulse path: inject / hw-sim / table-soft fire -> badge OR on the
 * global MSI-X Notification. Path tags attribute last pulse for stats.
 * greppable: MSI-X soft pulse path
 *
 * Driver-host wait path (soft != product IRQ cap mint; same shape as product):
 *   1) Soft-bind (optional): irq_msix_soft_user_bind / DDI_OP_IRQ_BIND
 *      records handle -> badge mask in a multi-slot soft table (note only;
 *      no hard IRQ in userspace; no CNode IRQ Notification mint).
 *      Multi-host safe residual: each DDI handle is one soft note slot;
 *      concurrent UDX hosts bind distinct handles (up to
 *      IRQ_MSIX_SOFT_USER_SLOTS). unbind(handle) / DDI CLOSE clears THAT
 *      handle only - never other hosts (never bind(0) for single CLOSE).
 *   2) Fire: irq_msix_soft_inject / PLATFORM_INFO op3 / table soft fire /
 *      hard IRQ -> notify_pulse on notify_msix_global().
 *   3) Wait: GJ_SYS_NOTIFY_WAIT
 *        arg0 = GJ_NOTIFY_WHICH_MSIX_GLOBAL (0)
 *        arg1 = badge mask  (e.g. GJ_MSIX_BADGE_SOFT | 0x7 smoke)
 *        arg2 = block       (0=poll, 1=block until matching pulse)
 *      -> returns CAS-cleared matched pending bits (unsigned in rax).
 * Product Notification mint honesty (Soft!=product dual license):
 *   product_notify_mint=OPEN  - no per-host IRQ Notification CNode mint
 *   product_irq_cap=OPEN      - delivery remains global MSI-X Notification
 *   soft_note_only=1          - DDI_OP_IRQ_BIND is handle->badge note only
 * greppable: irq_msix: soft user notify PASS
 * greppable: irq_msix: soft user bind
 * greppable: irq_msix: soft residual
 * greppable: irq_msix: soft residual lean
 * greppable: irq_msix: soft residual multi_host
 * greppable: irq_msix: soft residual multi_host PASS
 * greppable: irq_msix: soft residual user_bind
 * greppable: irq_msix: soft residual user_bind PASS
 * greppable: product_notify_mint=OPEN
 *
 * Soft user bind residual for DDI_OP_IRQ_BIND (Soft!=product dual license):
 *   path=DDI_OP_OPEN->DDI_OP_IRQ_BIND(handle,badge)->soft inject->
 *        GJ_SYS_NOTIFY_WAIT which=0->DDI_OP_CLOSE unbind(handle)
 *   Multi-host unbind-safe: unbind(one) never bind(0) clear-all on CLOSE.
 *   Product Notification mint OPEN (soft_note_only; no CNode IRQ mint).
 *   Rebind updates that handle only; concurrent hosts keep peer slots.
 *
 * Badge bits (OR into Notification pending):
 *   GJ_MSIX_BADGE_SOFT  - soft inject (bit 0)
 *   GJ_MSIX_BADGE_HW    - hw-sim pulse (bit 1)
 *   GJ_MSIX_BADGE_TBL(i)- soft table entry i (bit 2+i%61) so smoke masks
 *                         0x7 still observe table-soft fire of entry 0
 * UDX freestanding alignment: badge bit N -> IRQ line N in udx_run
 * (low 64 bits of the Notification word). Soft table is residual only.
 *
 * Order: call irq_msix_init after idt_init + APIC (+ notify_msix_init).
 * pci_msix soft table fire routes through this path when ready.
 *
 * Hazard H1 residual (IRQ stack smash / #PF I=1 wild RIP class):
 *   This unit NEVER calls net_eth_poll (hard handler, soft inject, pulse,
 *   table fire, exercise, init). Eth poll is run-loop only (scheduler_run).
 *   Freestanding net may stay poll-mode; MSI-X never forces IRQ eth poll.
 *   Soft != product DoD close; residual is UDX userspace IRQ hosts later.
 *   greppable: poll_mode_first=1 force_irq_eth_poll=0
 *   greppable: net_eth_poll=run_loop_only net_eth_irq=0
 *   greppable: net_eth_poll_from_msix=0
 *
 * Greppable product markers (keep stable):
 *   MSI-X soft pulse path
 *   notify: MSI-X IRQ PASS (with notify bind)
 *   irq_msix soft path exercise PASS
 *   irq_msix: soft user notify PASS
 *   irq_msix: soft residual
 *   irq_msix: soft residual lean
 *   irq_msix: soft residual lean PASS
 *   irq_msix: soft residual multi_host
 *   irq_msix: soft residual multi_host PASS
 *   irq_msix: soft residual user_bind
 *   irq_msix: soft residual user_bind PASS
 *   irq_msix: soft user bind
 *   product_notify_mint=OPEN
 *   product_irq_cap=OPEN
 *   soft_note_only=1
 */
#pragma once

#include <gj/types.h>

/** Product MSI-X delivery vector (IDT). Matches pci_msix_probe_log default. */
#define GJ_MSIX_IRQ_VEC 0x41u

/** Soft multi-slot soft-bind capacity (DDI handle -> badge mask notes). */
#define IRQ_MSIX_SOFT_USER_SLOTS 8u

/**
 * Lean residual honesty constants (Soft != product - Hazard H1).
 * Freestanding net may be poll-mode; MSI-X residual must not force IRQ eth.
 * Compile-time locked: force=0, poll_mode_first=1 (never flip without H1 review).
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 * greppable: net_eth_poll=run_loop_only net_eth_irq=0
 */
#define IRQ_MSIX_FORCE_IRQ_ETH_POLL 0u
#define IRQ_MSIX_POLL_MODE_FIRST    1u
/** H1: this unit never owns eth poll (run-loop only). Soft!=product. */
#define IRQ_MSIX_NET_ETH_IRQ        0u
/** H1: net_eth_poll never called from irq_msix.* (hard or soft path). */
#define IRQ_MSIX_NET_ETH_POLL_FROM  0u

/* Compile-time H1 locks (also asserted in irq_msix.c). Soft!=product. */
#if IRQ_MSIX_FORCE_IRQ_ETH_POLL != 0u
#error "H1: IRQ_MSIX_FORCE_IRQ_ETH_POLL must be 0"
#endif
#if IRQ_MSIX_POLL_MODE_FIRST != 1u
#error "H1: IRQ_MSIX_POLL_MODE_FIRST must be 1"
#endif
#if IRQ_MSIX_NET_ETH_IRQ != 0u
#error "H1: IRQ_MSIX_NET_ETH_IRQ must be 0"
#endif
#if IRQ_MSIX_NET_ETH_POLL_FROM != 0u
#error "H1: IRQ_MSIX_NET_ETH_POLL_FROM must be 0 (never call net_eth_poll)"
#endif

/** Soft-inject default badge bit (bit 0 = any / soft). */
#define GJ_MSIX_BADGE_SOFT (1ull << 0)
/** Hw-sim pulse badge bit (distinct from soft). */
#define GJ_MSIX_BADGE_HW (1ull << 1)
/**
 * Soft table entry -> badge bit. Entry 0 uses bit 2 so smoke masks 0x7
 * (bits 0..2) still observe table-soft fire of entry 0.
 */
#define GJ_MSIX_BADGE_TBL(idx) (1ull << (2u + ((u32)(idx) % 61u)))

/** Path tags for last-pulse attribution (stats only). */
#define GJ_MSIX_PATH_NONE  0u
#define GJ_MSIX_PATH_SOFT  1u
#define GJ_MSIX_PATH_HW    2u
#define GJ_MSIX_PATH_IRQ   3u
#define GJ_MSIX_PATH_TBL   4u

/**
 * Install IDT gate + bind global Notification.
 * Call after idt_init + APIC (+ notify_msix_init). Idempotent soft.
 */
void irq_msix_init(void);

/**
 * C handler for MSI-X vector (from isr stub); signals Notification + EOI.
 * Live device IRQs enter here; soft paths do not.
 */
void irq_msix_handler(void);

/**
 * Software inject (smoke / host inject without device fire).
 * OR @u64Badge into the MSI-X Notification; updates soft count / last path.
 * When a soft user bind is live and pending matches the bind mask, emits
 * greppable "irq_msix: soft user notify PASS" (once-shot lamp).
 */
void irq_msix_soft_inject(u64 u64Badge);

/**
 * Soft-record driver-host IRQ note: handle -> badge mask (multi-slot).
 * Soft != product: no CNode IRQ Notification mint; global MSI-X notify only.
 * Product Notification mint remains OPEN (soft_note_only=1).
 * DDI_OP_IRQ_BIND residual foundation (Soft!=product dual license):
 *   door passes live soft handle + badge; never handle 0 (bulk clear).
 *   Rebind updates an existing handle mask only (peer slots stay live).
 *   Multi-host: bind while others live is the concurrent UDX host shape.
 * u64BadgeMask 0 -> GJ_MSIX_BADGE_SOFT. u32Handle 0 clears all soft notes
 * (bulk clear only - DDI CLOSE must use unbind(handle) for multi-host safety).
 * Returns 0 if ready (or clear), -1 if IRQ path not ready or table full.
 * greppable path: ddi: soft irq note PASS (via DDI_OP_IRQ_BIND)
 * greppable: irq_msix: soft user bind
 * greppable: irq_msix: soft residual user_bind
 * greppable: product_notify_mint=OPEN
 */
int irq_msix_soft_user_bind(u32 u32Handle, u64 u64BadgeMask);

/**
 * Soft-clear one driver-host IRQ note by handle (multi-host safe).
 * Clears THAT handle only - other live host slots remain bound.
 * DDI_OP_CLOSE / DDI host lifecycle must call this (not bind(0,*)).
 * u32Handle 0 clears all (same as bind(0,*) - bulk only; not multi-host CLOSE).
 * Returns 0 always when ready enough to touch the soft table; -1 only if
 * table not usable (!ready and non-zero handle). Soft != product.
 * greppable: irq_msix: soft user unbind
 * greppable: irq_msix: soft residual multi_host
 */
int irq_msix_soft_user_unbind(u32 u32Handle);

/** Soft-bound primary DDI handle id (last bind; 0 = unbound); stats. */
u32 irq_msix_soft_user_handle(void);
/** Soft-bound primary badge wait mask (last bind); stats. */
u64 irq_msix_soft_user_mask(void);
/** Lifetime soft user-notify match hits (after inject with any bind live). */
u32 irq_msix_soft_user_notify_hits(void);
/** Live soft-bind slot count (0..IRQ_MSIX_SOFT_USER_SLOTS). */
u32 irq_msix_soft_user_slots_live(void);
/** Soft-bind table capacity (IRQ_MSIX_SOFT_USER_SLOTS). */
u32 irq_msix_soft_user_slots_cap(void);
/**
 * Multi-host unbind-safe residual hits (unbind(one) left other hosts live).
 * Soft residual telemetry only; Soft!=product; not a product gate.
 */
u32 irq_msix_soft_user_multi_host_safe(void);
/**
 * Non-zero after multi-host residual selftest PASS (once-shot exercise).
 * greppable: irq_msix: soft residual multi_host PASS
 */
u32 irq_msix_soft_user_multi_host_pass(void);
/**
 * Soft user rebind residual hits (DDI_OP_IRQ_BIND on already-bound handle).
 * Soft!=product; product Notification mint remains OPEN.
 */
u32 irq_msix_soft_user_rebinds(void);
/**
 * Soft multi-host bind residual hits (new bind while peer slot(s) live).
 * Soft residual only; Soft!=product; DDI IRQ_BIND concurrent-host shape.
 */
u32 irq_msix_soft_user_multi_host_binds(void);
/**
 * Non-zero after soft residual user_bind selftest PASS (once-shot).
 * greppable: irq_msix: soft residual user_bind PASS
 * Path: DDI_OP_IRQ_BIND residual - bind/rebind/multi-host/unbind-safe.
 */
u32 irq_msix_soft_user_bind_residual_pass(void);

/**
 * Soft pulse path: inject badge on the MSI-X Notification, track last badge
 * and path tag, return cumulative soft pulse-path count (0 if not ready).
 * greppable: MSI-X soft pulse path
 */
u32 irq_msix_soft_pulse_path(u64 u64Badge);

/**
 * Hw-sim pulse: Notification-side of a hardware IRQ without self-IPI/EOI
 * (early kmain self-IPI risks #DF). Returns cumulative hw pulse count.
 * Live device IRQs use the installed IDT gate -> irq_msix_handler.
 */
u32 irq_msix_hw_pulse(void);

/**
 * Soft table -> Notification pulse: fire soft MSI-X entry u16Idx via
 * pci_msix_soft_fire (PBA + mask gate) then confirm path stats.
 * Returns 1 if delivery attempted, 0 otherwise.
 */
u32 irq_msix_soft_table_pulse(u16 u16Idx);

/**
 * Soft path exercise: multi-badge OR inject + pending poll + table soft fire.
 * Greppable PASS. Call after irq_msix_init (or no-ops cleanly if not ready).
 * Returns non-zero on success.
 */
u32 irq_msix_soft_path_exercise(void);

/** Lifetime hard IRQ handler entries (stats). */
u32 irq_msix_count(void);
/** Lifetime soft injects (stats). */
u32 irq_msix_soft_count(void);
/** Lifetime hw-sim pulses (stats). */
u32 irq_msix_hw_count(void);
/** Lifetime soft pulse-path calls (stats). */
u32 irq_msix_soft_pulse_path_count(void);
/** Lifetime soft table pulses (stats). */
u32 irq_msix_table_pulse_count(void);
/** Last pulsed badge word (stats). */
u64 irq_msix_last_badge(void);
/** Last path tag GJ_MSIX_PATH_* (stats). */
u32 irq_msix_last_path(void);
/** Non-zero after successful irq_msix_init. */
int irq_msix_ready(void);
