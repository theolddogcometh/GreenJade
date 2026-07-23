/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX-shaped advisory file locks (fcntl F_SETLK / F_GETLK / F_SETLKW).
 * Byte-range RDLCK/WRLCK/UNLCK for Wine/Proton and lockf-shaped callers.
 * Soft multi-waiter F_SETLKW + range-split conflict detection (v1 deepen).
 * Pure C11; dual-licensed product surface.
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

struct gj_flock {
    i16 i16Type;   /* GJ_F_RDLCK / GJ_F_WRLCK / GJ_F_UNLCK */
    i16 i16Whence; /* 0 = SEEK_SET only */
    i64 i64Start;
    i64 i64Len; /* 0 = to EOF */
    u32 u32Pid;
};

void file_lock_init(void);

/**
 * F_SETLK-shaped: apply or unlock.
 * Returns 0, -LINUX_EAGAIN (nonblock conflict), -LINUX_ENOLCK (table full),
 * -LINUX_EBADF / -LINUX_EINVAL, -GJ_FLOCK_EDEADLK (soft wait cycle).
 * fBlock: non-zero → soft multi-waiter block until free (F_SETLKW).
 * Partial UNLCK/SETLK carves own ranges (split remnants; FLOCK_CONFLICT_SPLIT).
 */
i64 file_lock_set(i64 i64Fd, const struct gj_flock *pFl, int fBlock);

/**
 * F_GETLK: on conflict rewrite *pFlInOut with the blocking lock
 * (lowest start; write preferred on ties); otherwise set i16Type = GJ_F_UNLCK.
 */
i64 file_lock_get(i64 i64Fd, struct gj_flock *pFlInOut);

/** Active advisory lock count (smoke / stats). */
u32 file_lock_count(void);

/**
 * Soft F_SETLKW waiter count (stats). greppable: FLOCK_SOFT_MULTI_WAITER
 */
u32 file_lock_waiters(void);

/**
 * Soft wake generation: bumps on unlock / release / successful SET that
 * may free a range so waiters can observe progress.
 */
u32 file_lock_wake_gen(void);

/** Drop all locks held on i64Fd (close path). Returns slots cleared. */
u32 file_lock_release_fd(i64 i64Fd);

/** Drop all locks owned by u32Pid (exit path). Returns slots cleared. */
u32 file_lock_release_pid(u32 u32Pid);
