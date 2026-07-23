/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX-shaped advisory file locks (fcntl F_SETLK / F_GETLK / F_SETLKW).
 * Byte-range RDLCK/WRLCK/UNLCK for Wine/Proton and lockf-shaped callers.
 * Pure C11; dual-licensed product surface.
 */
#pragma once

#include <gj/types.h>

/* Linux flock type / whence (subset; SEEK_SET only) */
#define GJ_F_RDLCK 0
#define GJ_F_WRLCK 1
#define GJ_F_UNLCK 2

#define GJ_SEEK_SET 0

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
 * -LINUX_EBADF / -LINUX_EINVAL.
 * fBlock: non-zero → yield-spin until free (F_SETLKW).
 */
i64 file_lock_set(i64 i64Fd, const struct gj_flock *pFl, int fBlock);

/**
 * F_GETLK: on conflict rewrite *pFlInOut with the blocking lock;
 * otherwise set i16Type = GJ_F_UNLCK.
 */
i64 file_lock_get(i64 i64Fd, struct gj_flock *pFlInOut);

/** Active advisory lock count (smoke / stats). */
u32 file_lock_count(void);

/** Drop all locks held on i64Fd (close path). Returns slots cleared. */
u32 file_lock_release_fd(i64 i64Fd);

/** Drop all locks owned by u32Pid (exit path). Returns slots cleared. */
u32 file_lock_release_pid(u32 u32Pid);
