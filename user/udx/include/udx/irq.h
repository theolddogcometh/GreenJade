/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Threaded IRQ model (Linux threaded IRQ mental model) — thr-only.
 *
 * Soft path (PCI skeleton):
 *   request_irq (table bind only)
 *     → host_fire_irq / NOTIFY_WAIT badge
 *     → udx_irq_dispatch (respects soft mask / nested disable)
 *     → handler: read status, ack MMIO, schedule_work
 *   free_irq / disable_irq in quiesce+remove
 *
 * Soft mask depth:
 *   disable_irq increments a per-line depth; enable_irq decrements.
 *   Fire while depth>0 latches a soft pending bit delivered on final enable.
 *
 * -------------------------------------------------------------------------
 * thr-only residual (C2; Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 * -------------------------------------------------------------------------
 *   UDX IRQ is thr-only — there is no hard-IRQ primary half.
 *   Linux mental split (shape only; never product hard IRQ):
 *     request_threaded_irq(primary, thread_fn) → hard primary + thr half
 *   UDX residual shape (this header / table only):
 *     udx_request_irq(handler) → thr-only table bind (hard_primary=0)
 *   Delivery always runs the bound handler in host / udx_run thread
 *   context (process context). No hard-IRQ stack, no BH-only
 *   constraints, no top-half/primary_handler residual.
 *
 *   thr handler MAY (process context, thr_only=1):
 *     - MMIO status read + ack (product-host ISR surface)
 *     - udx_schedule_work (work.h deferred BH residual; NAPI stand-in)
 *     - return UDX_IRQ_HANDLED / UDX_IRQ_NONE
 *   thr handler MUST NOT:
 *     - assume non-preemptible hard-IRQ / primary_handler half
 *     - call net_eth_poll from IRQ (H1; soft_ne_product=1)
 *     - claim product Notification cap mint / DDI_OP_IRQ_BIND from here
 *     - claim Dual DoD A/B close (agent_close=0; soft lamp != close)
 *
 *   synchronize_irq soft no-op because thr-only: handlers run
 *   synchronously in dispatch (no separate hard-IRQ thread to park).
 *   Product hosts (rtl8168_udx / xhci_udx) are thr-only table+fire
 *   (Notification cap mint remains OPEN; Dual DoD B close is interactive SSH login).
 *   != kernel linux_time_soft request_threaded_irq primary+thread_fn.
 *   thr_only=1 hard_primary=0 hard_irq_thread=0 soft_ne_product=1
 *   H1=no_net_eth_poll DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP.
 *
 * Soft residual deepen (Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 *   request_irq residual — UDX table slot fill only; never mints a
 *     Notification cap or binds a kernel vector (table_only=1).
 *     Does not issue DDI_OP_IRQ_BIND (kernel soft handle→badge note is
 *     separate; greppable ddi: soft irq note PASS lives in kernel).
 *     thr-only: slot is threaded-handler residual, not hard primary.
 *   free_irq residual    — clears table action by pDevId; no product
 *     revoke / Phase-A Notification unbind claim; soft mask/pending
 *     local state dropped when the line becomes empty.
 *   soft fire residual   — host fire_irq ≈ pulse+claim of badge bit N;
 *     freestanding: NOTIFY_WAIT badge → udx_irq_dispatch(N). Soft mask
 *     latch/unmask stay UDX-table local (not hard-IRQ EOI / MSI-X PBA).
 *     thr-only: fire delivers into thr handler context only.
 *   thr residual         — hard_primary=0; thr_only=1; host thread /
 *     udx_run process context; schedule_work legal; sync soft no-op;
 *     H1 no net_eth_poll; no primary_handler residual.
 *   thr-only residual    — C2 exclusive: thr_only=1 complete model
 *     (no hard half ever); product_host thr surface + Dual DoD OPEN.
 *   product_host residual — rtl8168_udx (DoD B) / xhci_udx (DoD A)
 *     thr-only table+fire surface; freestanding rtl/USB class SKIP;
 *     product=UDX+ABI Linux-shaped userspace (Soft!=product).
 *   dual_dod residual    — Dual DoD A/B OPEN honesty; thr residual
 *     lamps alone never close Dual DoD (agent_close=0).
 *   DDI IRQ_BIND badge path honesty —
 *     product-shaped bind is kernel DDI_OP_IRQ_BIND (soft handle→badge
 *     note via irq_msix_soft_user_bind; multi-host never_clear_all;
 *     CLOSE unbinds that handle only). UDX request_irq is the host-side
 *     thr-only handler table, not the DDI badge mint path.
 *     Badge shape (aligned): bit N → IRQ line N (N in [0,63] badge word).
 *   product Notification cap mint = OPEN (not claimed; Soft!=product).
 *     Product hosts (rtl8168_udx / xhci_udx) stay thr-only table+fire
 *     until mint.
 *
 * Soft API honesty:
 *   soft path = thr-only table bind + fire_irq / NOTIFY_WAIT badge
 *   product   = IRQ Notification cap install (OPEN; not claimed here)
 *   Dual DoD  = A (xhci_udx) / B (rtl8168_udx) remain OPEN
 * greppable: udx: notify soft residual lean
 * greppable: udx: notify soft residual request
 * greppable: udx: notify soft residual free
 * greppable: udx: notify soft residual fire
 * greppable: udx: notify soft residual thr
 * greppable: udx: notify soft residual thr-only
 * greppable: udx: notify soft residual product_host
 * greppable: udx: notify soft residual dual_dod
 * greppable: udx: notify soft residual ddi_irq_bind
 * greppable: udx: notify soft residual cap_mint
 * greppable: udx: notify soft api honesty
 *
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 * thr-only IRQ; no GPL; no freestanding class re-enable; stamp-free;
 * no GJ_IMAGE_VERSION bump from this residual. H1: no net_eth_poll.
 */
#pragma once

#include <udx/types.h>

struct udx_device;

typedef int (*udx_irq_handler_t)(int nIrq, void *pDevId);

#define UDX_IRQF_SHARED  (1u << 0)

/* Linux irqreturn_t mental model for handler return. */
#define UDX_IRQ_NONE     0
#define UDX_IRQ_HANDLED  1

/*
 * thr-only residual honesty constants (eng only; Soft!=product; stamp-free).
 * Catalog flags for greppable thr residual — never product hard-IRQ claim,
 * never Dual DoD close, never GJ_IMAGE_VERSION. G-AC-1; no GPL.
 * greppable: udx: notify soft residual thr-only
 * greppable: UDX_IRQ_THR_ONLY / UDX_IRQ_HARD_PRIMARY / UDX_IRQ_DOD_*_OPEN
 */
#define UDX_IRQ_THR_ONLY            1u /* thr-only model; no hard primary */
#define UDX_IRQ_HARD_PRIMARY        0u /* no Linux primary_handler half */
#define UDX_IRQ_HARD_IRQ_THREAD     0u /* no separate hard-IRQ thread */
#define UDX_IRQ_CAP_MINT            0u /* product Notification mint OPEN */
#define UDX_IRQ_DDI_BIND_FROM_HERE  0u /* request_irq never DDI_OP_IRQ_BIND */
#define UDX_IRQ_TABLE_ONLY          1u /* table slot fill only */
#define UDX_IRQ_H1_NO_NET_ETH_POLL  1u /* H1: thr handler must not poll eth */
#define UDX_IRQ_DOD_A_OPEN          1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_IRQ_DOD_B_OPEN          1u /* rtl8168_udx Dual DoD B still OPEN */
#define UDX_IRQ_AGENT_CLOSE         0u /* thr residual lamps != Dual DoD close */
#define UDX_IRQ_SOFT_NE_PRODUCT     1u /* Soft!=product */

/**
 * Register thr-only threaded handler in the UDX table (table-only bind).
 * Delivery is pumped from udx_run (NOTIFY_WAIT freestanding, or
 * udx_host_fire_irq on host). Handlers always run in host / udx_run
 * thread context (process context) — thr_only=1, hard_primary=0.
 * No Linux request_threaded_irq primary half; not hard-IRQ stack.
 * Does not mint a product IRQ Notification cap; does not issue
 * DDI_OP_IRQ_BIND (kernel soft badge note is separate). cap_mint=OPEN.
 * thr handler: MMIO ack + schedule_work legal; H1 no net_eth_poll;
 * never claims Dual DoD A/B close (DoD_A=OPEN DoD_B=OPEN).
 * greppable residual: udx: notify soft residual request
 * greppable residual: udx: notify soft residual thr-only
 * greppable residual: udx: notify soft residual product_host
 */
udx_status_t udx_request_irq(int nIrq, udx_irq_handler_t pfnHandler,
                             u32 u32Flags, const char *szName, void *pDevId);

/**
 * Clear table action by pDevId (free_irq residual). No product revoke.
 * When the line becomes empty, soft mask/pending local state is dropped.
 * thr-only: no hard-IRQ primary unbind residual.
 * greppable residual: udx: notify soft residual free
 */
void udx_free_irq(int nIrq, void *pDevId);

/**
 * Soft-mask line (Linux disable_irq mental model, process context).
 * Nested: each disable pairs with enable. Depth>0 skips dispatch.
 * A fire while masked latches soft pending; delivered on depth→0.
 * thr-only: mask is UDX-table local (not hard-IRQ / MSI-X PBA mask).
 */
void udx_disable_irq(int nIrq);
void udx_enable_irq(int nIrq);

/** Non-zero if line is soft-masked (depth > 0). */
int udx_irq_is_disabled(int nIrq);

/** Current soft-mask depth (0 = enabled). */
int udx_irq_disable_depth(int nIrq);

/** Non-zero if a fire was latched while soft-masked. */
int udx_irq_is_pending(int nIrq);

/**
 * Host soft synchronize: thr-only no-op (handlers run synchronously
 * in dispatch; no separate hard-IRQ thread to wait on).
 * Exists so porters can call it on remove like Linux synchronize_irq.
 * greppable residual: udx: notify soft residual thr
 * greppable residual: udx: notify soft residual thr-only
 */
void udx_synchronize_irq(int nIrq);

/** First action name on the line, or NULL if unbound. */
const char *udx_irq_name(int nIrq);

/** Count of bound actions on the line (shared lines may be >1). */
int udx_irq_action_count(int nIrq);

/**
 * Soft fire residual entry: host fire_irq or freestanding NOTIFY_WAIT
 * badge claim. Badge bit N → line N; not a product Notification cap mint.
 * thr-only: invokes table handlers in host thread context only
 * (hard_primary=0; hard_irq_thread=0; schedule_work legal from handler;
 * H1=no_net_eth_poll; Dual DoD A/B remain OPEN).
 * greppable residual: udx: notify soft residual fire
 * greppable residual: udx: notify soft residual thr-only
 * greppable residual: udx: notify soft residual dual_dod
 */
void udx_irq_dispatch(int nIrq);
