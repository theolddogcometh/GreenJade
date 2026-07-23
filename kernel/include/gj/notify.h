/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification objects — badge bitmask pulse/wait (SECURITY / Apple channel).
 *
 * greppable: NOTIFY_BADGE_PULSE_WAIT
 * greppable: NOTIFY_SOFT_MULTI_WAITER
 *
 * Hard IRQ → pulse only (OR badge bits, optional soft multi-wake); no alloc,
 * no copy. Drivers batch work in userspace after notify_wait (UDX threaded IRQ).
 * Installable in a CNode as GJ_CAP_NOTIFICATION.
 *
 * Soft multi-waiter (v1 deepen, no redesign):
 *   - Multiple threads may block on the same notify object (tag=waiter).
 *   - Pulse wakes up to NOTIFY_SOFT_MULTI_MAX waiters; they race CAS-clear on
 *     u64Pending (matched badge bits go to one waiter; others re-sleep).
 *   - pWaiter is a non-authoritative hint only (fast IRQ “has waiter?” path).
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

struct gj_notify {
    struct gj_obj_hdr   hdr;         /* first: NOTIFICATION object header */
    u32                 u32Ready;
    u32                 u32Signals;  /* lifetime pulse count (wrap OK) */
    u64                 u64Pending;  /* OR of uncleared badges (atomic RMW) */
    u64                 u64LastBadge;
    struct gj_thread   *pWaiter;     /* soft multi-waiter hint (not exclusive) */
    u32                 u32Waiters;  /* soft multi-waiter count (atomic) */
};

void notify_init(struct gj_notify *pN);

/**
 * Pulse: OR badge into pending; soft multi-wake waiters.
 * greppable: NOTIFY_BADGE_PULSE
 * IRQ-safe (no alloc, atomics only).
 * Badge 0 is coalesced to bit 0 (bit 0 always means “any event”).
 *
 * notify_pulse is the product name; notify_signal remains the stable alias.
 */
void notify_pulse(struct gj_notify *pN, u64 u64Badge);
void notify_signal(struct gj_notify *pN, u64 u64Badge);

/**
 * Wait for any bit in mask (or any pending if mask==0).
 * greppable: NOTIFY_BADGE_WAIT
 * fBlock: block current thread until a matching pulse (soft multi-waiter OK).
 * Returns cleared bits (pending & mask). 0 if none / not ready / non-block miss.
 *
 * Matched bits are CAS-cleared so concurrent waiters do not double-claim.
 */
u64  notify_wait(struct gj_notify *pN, u64 u64Mask, int fBlock);

/** Non-blocking poll: equivalent to notify_wait(pN, mask, 0). */
u64  notify_poll(struct gj_notify *pN, u64 u64Mask);

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
 */
gj_status_t notify_install(struct gj_process *pProc, struct gj_notify *pN,
                           u16 u16Rights, struct gj_cap_ref *pOutRef);

/** Global MSI-X delivery Notification (vector 0x41 bind). */
struct gj_notify *notify_msix_global(void);
void notify_msix_init(void);
