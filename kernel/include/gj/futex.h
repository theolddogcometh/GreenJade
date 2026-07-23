/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Futex wait queues — Linux-true keying (G-FUT / G-MO-3):
 *   PRIVATE: key by (address space, user VA)
 *   shared:  key by physical address of the word (same PA ⇒ same key
 *            across aliasing maps / processes) — G-FUT-1 cross-proc
 *
 * Timeouts: absolute mono-nsec deadline; timer_tick → futex_timer_check
 * reaps with -ETIMEDOUT (G-FUT-2). Product path sleeps via thread_block
 * only — no busy-spin (G-FUT-3).
 *
 * Soft product surface (this header / futex.c):
 *   G-FUT-BITSET  — FUTEX_WAIT_BITSET / FUTEX_WAKE_BITSET
 *   G-FUT-ROBUST  — set/get_robust_list + soft exit OWNER_DIED wake
 *
 * Greppable comments / markers in kernel/sync/futex.c:
 *   futex: shared key PA
 *   futex: wait_bitset
 *   futex: wake_bitset
 *   futex: robust set/get/exit
 *   G-FUT-1 G-FUT-2 G-FUT-3 G-FUT-BITSET G-FUT-ROBUST
 */
#pragma once

#include <gj/types.h>

struct gj_process;
struct gj_thread;

/* ---- Linux futex opcodes (clean-room public ABI values) ----------------- */
#define GJ_FUTEX_WAIT            0
#define GJ_FUTEX_WAKE            1
#define GJ_FUTEX_FD              2 /* deprecated; not implemented */
#define GJ_FUTEX_REQUEUE         3 /* soft catalog only */
#define GJ_FUTEX_CMP_REQUEUE     4
#define GJ_FUTEX_WAKE_OP         5
#define GJ_FUTEX_LOCK_PI         6
#define GJ_FUTEX_UNLOCK_PI       7
#define GJ_FUTEX_TRYLOCK_PI      8
#define GJ_FUTEX_WAIT_BITSET     9  /* G-FUT-BITSET soft product */
#define GJ_FUTEX_WAKE_BITSET     10 /* G-FUT-BITSET soft product */
#define GJ_FUTEX_WAIT_REQUEUE_PI 11
#define GJ_FUTEX_CMP_REQUEUE_PI  12

#define GJ_FUTEX_PRIVATE_FLAG    128
#define GJ_FUTEX_CLOCK_REALTIME  256

/** Waiter / wake bitset that matches any other non-zero mask (Linux). */
#define GJ_FUTEX_BITSET_MATCH_ANY 0xffffffffu

/* Robust futex word bits (Linux public layout; soft G-FUT-ROBUST). */
#define GJ_FUTEX_WAITERS     0x80000000u
#define GJ_FUTEX_OWNER_DIED  0x40000000u
#define GJ_FUTEX_TID_MASK    0x3fffffffu

/** sizeof(struct robust_list_head) on x86_64 Linux ABI. */
#define GJ_FUTEX_ROBUST_LIST_HEAD_SZ 24u

/**
 * Stable wait-queue key built by futex_key_from_uaddr.
 *
 * Private: u8Private=1, u64As = CR3/AS id, u64Uaddr = VA, u64Phys = 0.
 * Shared:  u8Private=0, u64Phys = PA of the u32 word (G-FUT-1), u64As = 0,
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
 * else resolve PA via vmm under the active CR3 (shared / cross-process).
 * Requires uaddr 4-byte aligned; returns GJ_ERR_INVAL if not.
 * Shared PA resolution: G-FUT-1 (futex: shared key PA).
 */
gj_status_t futex_key_from_uaddr(struct gj_futex_key *pOut, u64 u64Uaddr,
                                 int fPrivate);

/**
 * Like futex_key_from_uaddr, but for shared keys may temporarily walk pProc's
 * CR3 so a cross-process builder can key the peer's mapping (G-FUT-1).
 * pProc NULL ⇒ same as futex_key_from_uaddr (active CR3 / current thr AS).
 */
gj_status_t futex_key_from_uaddr_proc(struct gj_futex_key *pOut, u64 u64Uaddr,
                                      int fPrivate, struct gj_process *pProc);

/** Non-zero if keys refer to the same wait queue (private AS+VA or shared PA). */
int futex_key_eq(const struct gj_futex_key *pA, const struct gj_futex_key *pB);

/**
 * Classic FUTEX_WAIT: if *pU32 still equals u32Val, park until wake/timeout.
 * Returns 0, or -LINUX_* (EAGAIN if value already changed, ETIMEDOUT, …).
 * u64DeadlineMonoNsec is absolute mono nsec, or 0 for no deadline.
 * Requires a current thread (G-FUT-3 — no product spin fallback).
 * Internally waits with GJ_FUTEX_BITSET_MATCH_ANY.
 */
i64 futex_wait(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
               u64 u64DeadlineMonoNsec);

/**
 * Soft FUTEX_WAIT_BITSET (G-FUT-BITSET / futex: wait_bitset):
 * Same as futex_wait, but only WAKE_BITSET with overlapping bits unblocks.
 * u32Bitset 0 → -LINUX_EINVAL (Linux).
 */
i64 futex_wait_bitset(volatile u32 *pU32, u32 u32Val,
                      const struct gj_futex_key *pKey, u64 u64DeadlineMonoNsec,
                      u32 u32Bitset);

/**
 * Classic FUTEX_WAKE: wake up to u32Count waiters matching pKey.
 * Equivalent to wake_bitset(..., GJ_FUTEX_BITSET_MATCH_ANY).
 * Returns number woken, or -LINUX_EINVAL.
 */
i64 futex_wake(const struct gj_futex_key *pKey, u32 u32Count);

/**
 * Soft FUTEX_WAKE_BITSET (G-FUT-BITSET / futex: wake_bitset):
 * Wake waiters whose stored bitset shares any bit with u32Bitset.
 * u32Bitset 0 → -LINUX_EINVAL.
 */
i64 futex_wake_bitset(const struct gj_futex_key *pKey, u32 u32Count,
                      u32 u32Bitset);

/** Reap timed-out waiters (call from timer IRQ / tick). IRQ-safe. */
void futex_timer_check(void);

/* ---- Soft robust list (G-FUT-ROBUST / futex: robust set/get/exit) -------- */

/**
 * Soft set_robust_list for the current thread: remember head VA + len.
 * Linux requires len == sizeof(robust_list_head); soft path accepts that
 * or any non-zero len ≤ 64 for bring-up. Returns GJ_OK / GJ_ERR_*.
 */
gj_status_t futex_set_robust_list(u64 u64Head, u64 u64Len);

/**
 * Soft get_robust_list: u32Tid 0 ⇒ current thread.
 * Writes head/len when non-NULL out pointers. GJ_ERR_NOENT if unset.
 */
gj_status_t futex_get_robust_list(u32 u32Tid, u64 *pHeadOut, u64 *pLenOut);

/**
 * Soft robust exit (futex: robust exit): walk the thread's robust list
 * (cycle-capped), OR FUTEX_OWNER_DIED into held words owned by this TID,
 * and wake one waiter per word (private key by thr AS, shared by PA).
 * Safe to call with NULL thr or empty list (returns 0).
 * Returns count of OWNER_DIED marks applied (best-effort soft).
 */
i64 futex_exit_robust_list(struct gj_thread *pThr);

/**
 * Drop all waiters owned by pThr (e.g. death path). Wakes them so they
 * do not hang forever. Returns number of slots cleared.
 */
u32 futex_cancel_thr(struct gj_thread *pThr);
