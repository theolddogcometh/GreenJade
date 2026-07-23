/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Notification objects — badge bitmask (SECURITY / Apple channel).
 *
 * Hard IRQ → signal only (OR badge bits, optional wake); no alloc, no copy.
 * Drivers batch work in userspace after notify_wait (UDX threaded IRQ).
 * Installable in a CNode as GJ_CAP_NOTIFICATION.
 */
#pragma once

#include <gj/cap.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

struct gj_notify {
    struct gj_obj_hdr   hdr;       /* first: NOTIFICATION object header */
    u32                 u32Ready;
    u32                 u32Signals;  /* lifetime pulse count (wrap OK) */
    u64                 u64Pending;  /* OR of uncleared badges (atomic RMW) */
    u64                 u64LastBadge;
    struct gj_thread   *pWaiter;     /* single waiter (v1) */
};

void notify_init(struct gj_notify *pN);

/**
 * OR badge into pending; wake one waiter. IRQ-safe (no alloc, atomics only).
 * Badge 0 is coalesced to bit 0 (bit 0 always means “any event”).
 */
void notify_signal(struct gj_notify *pN, u64 u64Badge);

/**
 * Wait for any bit in mask (or any pending if mask==0).
 * fBlock: block current thread until a matching signal.
 * Returns cleared bits (pending & mask). 0 if none / not ready / non-block miss.
 */
u64  notify_wait(struct gj_notify *pN, u64 u64Mask, int fBlock);

/** Non-blocking poll: equivalent to notify_wait(pN, mask, 0). */
u64  notify_poll(struct gj_notify *pN, u64 u64Mask);

/** Lifetime signal count (stats); 0 if object missing. */
u32  notify_signals(const struct gj_notify *pN);

/** Current uncleared badge bits (stats); 0 if object missing. */
u64  notify_pending(const struct gj_notify *pN);

/** Non-zero if ready and LIVE. */
int  notify_is_live(const struct gj_notify *pN);

/**
 * Wake waiter without posting a badge (teardown). Wait returns 0 unless
 * pending bits remain.
 */
void notify_abort_waiter(struct gj_notify *pN);

/**
 * Mark notification DEAD (obj_hdr + ready clear) and wake waiter.
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
