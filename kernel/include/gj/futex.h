/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Futex wait queues — Linux-true keying (G-FUT / G-MO-3):
 *   PRIVATE: key by (address space, user VA)
 *   shared:  key by physical address of the word (same PA ⇒ same key
 *            across aliasing maps / processes)
 *
 * Timeouts: absolute mono-nsec deadline; timer_tick → futex_timer_check
 * reaps with -ETIMEDOUT (G-FUT-2). Product path sleeps via thread_block
 * only — no busy-spin (G-FUT-3).
 */
#pragma once

#include <gj/types.h>

#define GJ_FUTEX_WAIT 0
#define GJ_FUTEX_WAKE 1
#define GJ_FUTEX_PRIVATE_FLAG 128

/**
 * Stable wait-queue key built by futex_key_from_uaddr.
 *
 * Private: u8Private=1, u64As = CR3/AS id, u64Uaddr = VA, u64Phys = 0.
 * Shared:  u8Private=0, u64Phys = PA of the u32 word, u64As = 0,
 *          u64Uaddr kept for diagnostics only.
 */
struct gj_futex_key {
    u64 u64Phys;   /* shared: PA of word; private: 0 */
    u64 u64Uaddr;  /* user VA (private key component; shared: diagnostic) */
    u64 u64As;     /* private: address-space id (CR3); shared: 0 */
    u8  u8Private;
    u8  u8Pad[7];
};

void futex_init(void);

/**
 * Build key: fPrivate non-zero ⇒ private (VA + current AS);
 * else resolve PA via vmm (shared / cross-process).
 * Requires uaddr 4-byte aligned; returns GJ_ERR_INVAL if not.
 */
gj_status_t futex_key_from_uaddr(struct gj_futex_key *pOut, u64 u64Uaddr,
                                 int fPrivate);

/**
 * Classic FUTEX_WAIT: if *pU32 still equals u32Val, park until wake/timeout.
 * Returns 0, or -LINUX_* (EAGAIN if value already changed, ETIMEDOUT, …).
 * u64DeadlineMonoNsec is absolute mono nsec, or 0 for no deadline.
 * Requires a current thread (G-FUT-3 — no product spin fallback).
 */
i64 futex_wait(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
               u64 u64DeadlineMonoNsec);

/**
 * Classic FUTEX_WAKE: wake up to u32Count waiters matching pKey.
 * Returns number woken, or -LINUX_EINVAL.
 */
i64 futex_wake(const struct gj_futex_key *pKey, u32 u32Count);

/** Reap timed-out waiters (call from timer IRQ / tick). IRQ-safe. */
void futex_timer_check(void);
