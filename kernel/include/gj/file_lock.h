/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX-shaped advisory file locks (fcntl F_SETLK / F_GETLK / F_SETLKW).
 * Byte-range RDLCK/WRLCK/UNLCK for Wine/Proton and lockf-shaped callers.
 * Pure C11 freestanding; dual MIT OR Apache-2.0 product surface.
 *
 * Soft multi-waiter F_SETLKW + range-split conflict detection (v1 deepen):
 *   - FLOCK_CONFLICT_SPLIT: partial UNLCK/SETLK carves own ranges; leaves
 *     non-overlapped remnants; adjacent same-type coalesce
 *   - FLOCK_SOFT_MULTI_WAITER: register waiters, thread_block/wake, gen bump
 *   - Soft deadlock: short cycle over waiter table → -GJ_FLOCK_EDEADLK
 *   - GETLK: lowest-start conflict; write preferred on equal start
 *   - Overflow-safe range ends; len 0 = to EOF (exclusive end sentinel)
 *
 * Capacity (file_lock.c — soft product limits, not wire ABI):
 *   GJ_FLOCK_MAX=32 active ranges; GJ_FLOCK_MAX_WAITERS=8
 *   Soft multi-wake budget per unlock/release (bounded, not a hard queue)
 *
 * Concurrency: ticket-free spinlock around table; pure C11 atomics not
 * required for the lock slots themselves. Close/exit paths must call
 * release_fd / release_pid so waiters observe wake_gen progress.
 *
 * greppable: FLOCK_SOFT_MULTI_WAITER
 * greppable: FLOCK_CONFLICT_SPLIT
 */
#pragma once

#include <gj/types.h>

/* Linux flock type / whence (subset; SEEK_SET only) */
#define GJ_F_RDLCK 0
#define GJ_F_WRLCK 1
#define GJ_F_UNLCK 2

#define GJ_SEEK_SET 0

/* Linux EDEADLK (not always in the cold ABI header). Soft cycle on SETLKW. */
#define GJ_FLOCK_EDEADLK 35

/**
 * Linux flock-shaped range descriptor (fcntl F_SETLK / F_GETLK).
 * i16Whence: only SEEK_SET (0) is accepted for bring-up.
 * i64Len: 0 means "to EOF" (infinity).
 * u32Pid: owner for conflict / GETLK rewrite / release_pid.
 */
struct gj_flock {
    i16 i16Type;   /* GJ_F_RDLCK / GJ_F_WRLCK / GJ_F_UNLCK */
    i16 i16Whence; /* 0 = SEEK_SET only */
    i64 i64Start;
    i64 i64Len; /* 0 = to EOF */
    u32 u32Pid;
};

/** Zero lock + waiter tables, init spinlock. Safe to re-call soft. */
void file_lock_init(void);

/**
 * F_SETLK-shaped: apply or unlock.
 * Returns 0, -LINUX_EAGAIN (nonblock conflict), -LINUX_ENOLCK (table full),
 * -LINUX_EBADF / -LINUX_EINVAL, -GJ_FLOCK_EDEADLK (soft wait cycle).
 * fBlock: non-zero → soft multi-waiter block until free (F_SETLKW).
 * Partial UNLCK/SETLK carves own ranges (split remnants; FLOCK_CONFLICT_SPLIT).
 * Same-pid overlapping SET replaces/carves; cross-pid write conflicts block
 * or EAGAIN. Successful SET/unlock bumps wake_gen for waiters.
 */
i64 file_lock_set(i64 i64Fd, const struct gj_flock *pFl, int fBlock);

/**
 * F_GETLK: on conflict rewrite *pFlInOut with the blocking lock
 * (lowest start; write preferred on ties); otherwise set i16Type = GJ_F_UNLCK.
 * Returns 0 or -LINUX_EINVAL / -LINUX_EBADF.
 */
i64 file_lock_get(i64 i64Fd, struct gj_flock *pFlInOut);

/** Active advisory lock count (smoke / stats; 0..GJ_FLOCK_MAX). */
u32 file_lock_count(void);

/**
 * Soft F_SETLKW waiter count (stats). greppable: FLOCK_SOFT_MULTI_WAITER
 */
u32 file_lock_waiters(void);

/**
 * Soft wake generation: bumps on unlock / release / successful SET that
 * may free a range so waiters can observe progress without spurious sleep.
 */
u32 file_lock_wake_gen(void);

/**
 * Drop all locks held on i64Fd (close path). Returns slots cleared.
 * Soft multi-wakes waiters and bumps wake_gen when any range freed.
 */
u32 file_lock_release_fd(i64 i64Fd);

/**
 * Drop all locks owned by u32Pid (exit path). Returns slots cleared.
 * Same wake policy as release_fd.
 */
u32 file_lock_release_pid(u32 u32Pid);
