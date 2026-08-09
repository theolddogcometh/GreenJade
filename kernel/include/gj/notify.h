/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification objects - badge bitmask pulse/wait (SECURITY / Apple channel).
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * greppable: NOTIFY_BADGE_PULSE_WAIT
 * greppable: NOTIFY_SOFT_MULTI_WAITER
 * greppable: NOTIFY_EVENT_POST
 * greppable: NOTIFY_WAKER_KICK
 * greppable: NOTIFY_WAKER_DRAIN
 * greppable: NOTIFY_WAIT_RESIDUAL
 * greppable: NOTIFY_LEAN_RESIDUAL
 *
 * UDX IRQ / user notify path (product-shaped residual; Soft!=product):
 *   hard IRQ / soft inject -> notify_pulse (OR badge; optional soft multi-wake)
 *   userspace UDX         -> GJ_SYS_NOTIFY_WAIT / notify_wait (CAS-clear mask)
 *   batch work after wait (threaded IRQ model). No alloc/copy in hard IRQ.
 *   G-AC-1: no Linux .ko product AC; delivery is Notification badge, not .ko.
 *   Product IRQ Notification mint OPEN (badge delivery; not .ko AC).
 *
 * Event residual (producer):
 *   notify_pulse / notify_event / notify_signal -> OR pending, soft multi-wake
 * Wait residual (UDX ISR consumer reaping; greppable NOTIFY_WAIT_RESIDUAL):
 *   notify_wait / notify_poll  - CAS-clear matched bits; park on TAG_WAITER
 *   ISR shape: pulse(badge) -> wait(mask, fBlock=0|1) reaps bits for UDX
 *   freestanding UDX pumps non-blocking wait; block path parks soft multi
 * Waker residual (consumer park / kick):
 *   notify_wake                - kick without badge (quiesce / peer)
 *   notify_wake_drain          - bounded multi-round kick (abort/mark_dead)
 *
 * Soft multi-waiter (v1 deepen, no redesign):
 *   - Multiple threads may block on the same notify object (tag=waiter).
 *   - Pulse wakes up to NOTIFY_SOFT_MULTI_MAX waiters; they race CAS-clear on
 *     u64Pending (matched badge bits go to one waiter; others re-sleep).
 *   - pWaiter is a non-authoritative hint only (fast IRQ "has waiter?" path).
 *   - u32Waiters is an atomic soft count for stats / has-waiter.
 * Soft multi-waiter != multi-process notify product. Soft != MIG REPLY product.
 * Soft != product. Dual MIT OR Apache-2.0. Lean residual only.
 *
 * Object lifecycle:
 *   notify_init -> LIVE + ready; notify_mark_dead -> DEAD + soft multi-wake
 *   (gj_obj_revoke_begin when still LIVE so CNode hygiene can follow).
 *   notify_abort_waiter soft multi-wakes without posting a badge.
 *
 * MSI-X bind:
 *   notify_msix_global() is the product delivery Notification for vector
 *   GJ_MSIX_IRQ_VEC (see irq_msix.h); pulse from hard IRQ + soft inject.
 *
 * Protocol (badge pulse / wait):
 *   producer  notify_pulse(badge)  -> OR pending, soft multi-wake
 *   consumer  notify_wait(mask,1)  -> CAS-clear matched bits, return them
 *   poll      notify_poll(mask)    -> wait non-blocking
 *   Badge 0 coalesces to bit 0 ("any event").
 */
#pragma once

#include <gj/cap.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

/*
 * Soft multi-waiter wake budget per pulse/abort (bounded; not a hard queue).
 * greppable: NOTIFY_SOFT_MULTI_MAX
 */
#define NOTIFY_SOFT_MULTI_MAX 8u

/*
 * Wait key tag for thread_block / thread_wake on a notify object.
 * greppable: NOTIFY_TAG_WAITER
 */
#define NOTIFY_TAG_WAITER 1u

/**
 * Notification object (first field = obj_hdr for CNode / revoke).
 * u64Pending is atomic RMW; other fields soft/stats unless noted.
 */
struct gj_notify {
    struct gj_obj_hdr   hdr;         /* first: NOTIFICATION object header */
    u32                 u32Ready;
    u32                 u32Signals;  /* lifetime pulse count (wrap OK) */
    u64                 u64Pending;  /* OR of uncleared badges (atomic RMW) */
    u64                 u64LastBadge;
    struct gj_thread   *pWaiter;     /* soft multi-waiter hint (not exclusive) */
    u32                 u32Waiters;  /* soft multi-waiter count (atomic) */
};

/** Zero object, init hdr LIVE, mark ready. Null pN is a no-op. */
void notify_init(struct gj_notify *pN);

/**
 * Pulse: OR badge into pending; soft multi-wake waiters.
 * greppable: NOTIFY_BADGE_PULSE
 * IRQ-safe (no alloc, atomics only).
 * Badge 0 is coalesced to bit 0 (bit 0 always means "any event").
 * No-op if null / not ready / DEAD.
 *
 * notify_pulse is the product name; notify_signal remains the stable alias.
 * Hard IRQ / MSI-X inject path for UDX userspace NOTIFY_WAIT reaping.
 */
void notify_pulse(struct gj_notify *pN, u64 u64Badge);
void notify_signal(struct gj_notify *pN, u64 u64Badge);

/**
 * Event residual: same product path as pulse (door / UDX async completion).
 * greppable: NOTIFY_EVENT_POST
 * IRQ-safe via notify_pulse. Named for completion call sites.
 */
void notify_event(struct gj_notify *pN, u64 u64Badge);

/**
 * Wait residual for UDX ISR path: reap any bit in mask (or any if mask==0).
 * greppable: NOTIFY_BADGE_WAIT
 * greppable: NOTIFY_WAIT_RESIDUAL
 * fBlock: block current thread until a matching pulse (soft multi-waiter OK).
 * Returns cleared bits (pending & mask). 0 if none / not ready / non-block miss.
 *
 * Matched bits are CAS-cleared so concurrent waiters do not double-claim.
 * Product path after hard IRQ / soft inject pulse:
 *   UDX freestanding: non-blocking wait (fBlock=0) via GJ_SYS_NOTIFY_WAIT
 *   UDX block path:   fBlock=1 parks on TAG_WAITER until pulse multi-wake
 * Soft!=product; product IRQ Notification mint OPEN (badge, not .ko).
 */
u64  notify_wait(struct gj_notify *pN, u64 u64Mask, int fBlock);

/**
 * Non-blocking poll: equivalent to notify_wait(pN, mask, 0).
 * Wait residual alias for UDX ISR reap (same CAS-clear path).
 */
u64  notify_poll(struct gj_notify *pN, u64 u64Mask);

/**
 * Waker residual: soft multi-wake without posting badge bits.
 * greppable: NOTIFY_WAKER_KICK
 * Door teardown / UDX quiesce. Waiters re-sample pending / live.
 * u32Max 0 -> NOTIFY_SOFT_MULTI_MAX. Returns count woken (0 if none/null).
 */
u32  notify_wake(struct gj_notify *pN, u32 u32Max);

/**
 * Bounded multi-round waker drain (abort/mark_dead residual).
 * greppable: NOTIFY_WAKER_DRAIN
 * Not hard-IRQ. Returns total woken across rounds.
 */
u32  notify_wake_drain(struct gj_notify *pN);

/** Lifetime pulse/signal count (stats); 0 if object missing. */
u32  notify_signals(const struct gj_notify *pN);

/** Current uncleared badge bits (stats); 0 if object missing. */
u64  notify_pending(const struct gj_notify *pN);

/** Last pulsed badge word (stats); 0 if object missing. */
u64  notify_last_badge(const struct gj_notify *pN);

/** Soft multi-waiter count (stats); 0 if object missing. */
u32  notify_waiters(const struct gj_notify *pN);

/** Non-zero if ready and LIVE. */
int  notify_is_live(const struct gj_notify *pN);

/**
 * Soft multi-wake waiters without posting a badge (teardown).
 * Wait returns 0 unless pending bits remain.
 * greppable: NOTIFY_ABORT_SOFT_MULTI
 * Uses waker drain so >MULTI_MAX waiters are not left parked.
 */
void notify_abort_waiter(struct gj_notify *pN);

/**
 * Mark notification DEAD (obj_hdr + ready clear) and soft multi-wake waiters.
 * Uses gj_obj_revoke_begin when still LIVE so CNode hygiene can follow.
 */
void notify_mark_dead(struct gj_notify *pN);

/**
 * Install notify as GJ_CAP_NOTIFICATION into process CNode.
 * Default rights if u16Rights == 0: READ | WAIT | IDENTIFY.
 * Returns GJ_OK or GJ_ERR_*; fills *pOutRef on success when non-NULL.
 */
gj_status_t notify_install(struct gj_process *pProc, struct gj_notify *pN,
                           u16 u16Rights, struct gj_cap_ref *pOutRef);

/**
 * Global MSI-X delivery Notification (vector 0x41 bind).
 * Valid after notify_msix_init(); may return non-NULL static even before
 * ready - callers should notify_is_live / irq_msix_ready as needed.
 * UDX / irq_msix product delivery object (badge pulse -> NOTIFY_WAIT).
 */
struct gj_notify *notify_msix_global(void);

/** Init global MSI-X Notification object (idempotent soft). */
void notify_msix_init(void);
