/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Futex wait queues — Linux-true keying (G-FUT / G-MO-3).
 *
 * Scope
 * -----
 * In-kernel wait/wake on a 32-bit user (or smoke kernel) word. Keys match
 * Linux private vs shared semantics so aliasing maps and cross-process
 * shared memory share one queue when they should. Product path parks via
 * thread_block + schedule only — never busy-spin (G-FUT-3). Fixed waiter
 * table (no heap); table full → -LINUX_ENOMEM.
 *
 * Keying (G-FUT-1)
 * ----------------
 *   PRIVATE (FUTEX_PRIVATE_FLAG / fPrivate != 0):
 *     key = (address space id, user VA)
 *     AS id prefers process CR3 (stable across temporary kernel CR3 loads),
 *     else thread CR3, else active CPU CR3 (boot / no-process threads).
 *   Shared (no private flag):
 *     key = physical address of the u32 word (page frame + word offset).
 *     Same PA ⇒ same queue across aliasing maps / processes.
 *     VA kept on the key for diagnostics only; equality ignores VA.
 *     PA via vmm walk under the target CR3; low identity fallback (PA==VA
 *     below 1 GiB) for boot/kernel BSS smoke when the leaf walk fails.
 *
 * Wait path (lost-wake safe)
 * --------------------------
 *   1. Fast load: *uaddr != expected → -LINUX_EAGAIN (no lock).
 *   2. irqsave-lock waiter table; claim slot; recheck *uaddr under lock.
 *   3. Register thread_block on the slot *before* unlock so concurrent
 *      wake/timer cannot miss this waiter.
 *   4. Unlock, schedule(); on resume free slot; return 0 or -LINUX_ETIMEDOUT.
 *   Value changes after enqueue may cancel and return -LINUX_EAGAIN without
 *   sleeping. Immediate past-deadline → -LINUX_ETIMEDOUT without park.
 *
 * Wake / timer
 * ------------
 *   Wake and futex_timer_check hold the same irqsave lock as wait (G-SMP /
 *   SECURITY_CORE §4) so IRQ tick cannot race table surgery.
 *   Timeouts: absolute mono-nsec deadline (timer_mono_nsec); 0 = no deadline.
 *   timer_tick / timer_tick_apic → futex_timer_check reaps with ETIMEDOUT
 *   (G-FUT-2). Wait object for thread_block/wake is the futex_waiter slot
 *   itself; block tag 0.
 *
 * Soft product surface (this header / futex.c)
 * --------------------------------------------
 *   G-FUT-1         — shared key by PA (cross-proc / alias)
 *   G-FUT-2         — absolute mono timeout reaped on tick
 *   G-FUT-3         — sleep via thread_block only (no product spin)
 *   G-FUT-BITSET    — FUTEX_WAIT_BITSET / FUTEX_WAKE_BITSET
 *   G-FUT-ROBUST    — set/get_robust_list + soft exit OWNER_DIED wake
 *
 * Opcode catalog below is clean-room public Linux ABI numbering. Only WAIT,
 * WAKE, WAIT_BITSET, WAKE_BITSET, and robust-list helpers are product-active;
 * REQUEUE / PI / FD remain soft catalog (not implemented here).
 *
 * Loads/stores of the futex word use SMAP-safe user_load_u32 / user_store_u32
 * with a direct-load smoke fallback for kernel BSS tests.
 *
 * greppable: futex: shared key PA
 * greppable: futex: wait_bitset
 * greppable: futex: wake_bitset
 * greppable: futex: robust set/get/exit
 * greppable: G-FUT-1 G-FUT-2 G-FUT-3 G-FUT-BITSET G-FUT-ROBUST
 * greppable: G-MO-3 G-SMP
 */
#pragma once

#include <gj/types.h>

struct gj_process;
struct gj_thread;

/* ---- Linux futex opcodes (clean-room public ABI values) ----------------- */
/*
 * Numeric values match the Linux uapi futex op field (low bits). OR with
 * GJ_FUTEX_PRIVATE_FLAG / GJ_FUTEX_CLOCK_REALTIME at the syscall edge as on
 * Linux. Kernel helpers take fPrivate / absolute mono deadlines separately.
 */
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

/** OR into op: private (per-AS) keying; same numeric value as Linux. */
#define GJ_FUTEX_PRIVATE_FLAG    128
/**
 * OR into op: realtime clock for absolute timeout at the Linux edge.
 * Kernel wait path still stores absolute mono nsec after conversion.
 */
#define GJ_FUTEX_CLOCK_REALTIME  256

/**
 * Waiter / wake bitset that matches any other non-zero mask (Linux).
 * Classic FUTEX_WAIT / FUTEX_WAKE use this internally.
 */
#define GJ_FUTEX_BITSET_MATCH_ANY 0xffffffffu

/*
 * Robust futex word bits (Linux public layout; soft G-FUT-ROBUST).
 * Low bits: owner TID; high bits: waiters present / owner died.
 */
#define GJ_FUTEX_WAITERS     0x80000000u
#define GJ_FUTEX_OWNER_DIED  0x40000000u
#define GJ_FUTEX_TID_MASK    0x3fffffffu

/** sizeof(struct robust_list_head) on x86_64 Linux ABI. */
#define GJ_FUTEX_ROBUST_LIST_HEAD_SZ 24u

/**
 * Stable wait-queue key built by futex_key_from_uaddr(_proc).
 *
 * Private: u8Private=1, u64As = CR3/AS id, u64Uaddr = VA, u64Phys = 0.
 * Shared:  u8Private=0, u64Phys = PA of the u32 word (G-FUT-1), u64As = 0,
 *          u64Uaddr kept for diagnostics only.
 *
 * Equality (futex_key_eq): private matches AS+VA; shared matches non-zero PA
 * only (empty phys never matches, accidental zero keys stay isolated).
 * greppable: G-FUT-1
 */
struct gj_futex_key {
    u64 u64Phys;   /* shared: PA of word; private: 0 */
    u64 u64Uaddr;  /* user VA (private key component; shared: diagnostic) */
    u64 u64As;     /* private: address-space id (CR3); shared: 0 */
    u8  u8Private;
    u8  u8Pad[7];
};

/** Zero waiter/robust tables and init the irqsave table lock. */
void futex_init(void);

/**
 * Build key: fPrivate non-zero ⇒ private (VA + current AS);
 * else resolve PA via vmm under the active / thr process CR3 (shared).
 *
 * Requires uaddr 4-byte aligned; returns GJ_ERR_INVAL if not or pOut NULL.
 * Shared PA resolution failure → GJ_ERR_FAULT.
 * Shared PA path: G-FUT-1 (futex: shared key PA).
 */
gj_status_t futex_key_from_uaddr(struct gj_futex_key *pOut, u64 u64Uaddr,
                                 int fPrivate);

/**
 * Like futex_key_from_uaddr, but for shared keys may temporarily walk pProc's
 * CR3 so a cross-process builder can key the peer's mapping (G-FUT-1).
 * Restores prior CR3 before return. pProc NULL ⇒ same as futex_key_from_uaddr
 * (active CR3 / current thr AS, with thr-process CR3 preference when mid-path
 * kernel CR3 is loaded). Private keys use pProc->u64Cr3 when set.
 */
gj_status_t futex_key_from_uaddr_proc(struct gj_futex_key *pOut, u64 u64Uaddr,
                                      int fPrivate, struct gj_process *pProc);

/**
 * Non-zero if keys refer to the same wait queue (private AS+VA or shared PA).
 * NULL either side → 0. Shared zero-PA keys never equal.
 */
int futex_key_eq(const struct gj_futex_key *pA, const struct gj_futex_key *pB);

/**
 * Classic FUTEX_WAIT: if *pU32 still equals u32Val, park until wake/timeout.
 *
 * Returns 0 on wake, or -LINUX_* :
 *   -LINUX_EAGAIN     value already changed (before or under lock)
 *   -LINUX_ETIMEDOUT  deadline reached (immediate or via futex_timer_check)
 *   -LINUX_EINVAL     nulls, bad alignment, shared zero PA, no current thr
 *   -LINUX_ENOMEM     waiter table full
 *
 * u64DeadlineMonoNsec is absolute timer_mono_nsec, or 0 for no deadline.
 * Requires a current thread (G-FUT-3 — no product spin fallback).
 * Internally waits with GJ_FUTEX_BITSET_MATCH_ANY.
 */
i64 futex_wait(volatile u32 *pU32, u32 u32Val, const struct gj_futex_key *pKey,
               u64 u64DeadlineMonoNsec);

/**
 * Soft FUTEX_WAIT_BITSET (G-FUT-BITSET / futex: wait_bitset):
 * Same as futex_wait, but only WAKE_BITSET with overlapping bits unblocks.
 * u32Bitset 0 → -LINUX_EINVAL (Linux). Stores bitset on the waiter slot.
 */
i64 futex_wait_bitset(volatile u32 *pU32, u32 u32Val,
                      const struct gj_futex_key *pKey, u64 u64DeadlineMonoNsec,
                      u32 u32Bitset);

/**
 * Classic FUTEX_WAKE: wake up to u32Count waiters matching pKey.
 * Equivalent to wake_bitset(..., GJ_FUTEX_BITSET_MATCH_ANY).
 * Returns number woken (0 if count 0), or -LINUX_EINVAL (null key / shared
 * zero PA).
 */
i64 futex_wake(const struct gj_futex_key *pKey, u32 u32Count);

/**
 * Soft FUTEX_WAKE_BITSET (G-FUT-BITSET / futex: wake_bitset):
 * Wake waiters whose stored bitset shares any bit with u32Bitset.
 * u32Bitset 0 → -LINUX_EINVAL. Match is (waiter_bitset & wake_bitset) != 0.
 */
i64 futex_wake_bitset(const struct gj_futex_key *pKey, u32 u32Count,
                      u32 u32Bitset);

/**
 * Reap timed-out waiters (call from timer IRQ / tick). IRQ-safe.
 * Compares timer_mono_nsec against each waiting slot's absolute deadline;
 * marks timed out and thread_wake under the futex irqsave lock.
 * No-op if timer not ready. Must not allocate (G-FUT-2).
 */
void futex_timer_check(void);

/* ---- Soft robust list (G-FUT-ROBUST / futex: robust set/get/exit) -------- */

/**
 * Soft set_robust_list for the current thread: remember head VA + len.
 * Linux requires len == sizeof(robust_list_head) (GJ_FUTEX_ROBUST_LIST_HEAD_SZ);
 * soft path accepts that or any non-zero len ≤ 64 for bring-up.
 * Returns GJ_OK / GJ_ERR_* (no current thr, bad args, robust slot full).
 * greppable: futex: robust set/get/exit
 */
gj_status_t futex_set_robust_list(u64 u64Head, u64 u64Len);

/**
 * Soft get_robust_list: u32Tid 0 ⇒ current thread.
 * Writes head/len when non-NULL out pointers.
 * GJ_ERR_NOENT if that TID has no robust head recorded; GJ_ERR_* on bad args.
 */
gj_status_t futex_get_robust_list(u32 u32Tid, u64 *pHeadOut, u64 *pLenOut);

/**
 * Soft robust exit (futex: robust exit): walk the thread's robust list
 * (cycle-capped walk so a corrupt ring cannot hang), OR FUTEX_OWNER_DIED
 * into held words owned by this TID, and wake one waiter per word
 * (private key by thr AS, shared by PA of the word).
 * Safe to call with NULL thr or empty list (returns 0).
 * Returns count of OWNER_DIED marks applied (best-effort soft).
 * greppable: G-FUT-ROBUST
 */
i64 futex_exit_robust_list(struct gj_thread *pThr);

/**
 * Drop all waiters owned by pThr (e.g. death path). Wakes them so they
 * do not hang forever. Clears slots under the futex lock.
 * Returns number of slots cleared. NULL thr → 0.
 */
u32 futex_cancel_thr(struct gj_thread *pThr);
