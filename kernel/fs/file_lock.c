/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Advisory byte-range locks for wine/Proton file locking.
 * Per-fd ranges with RDLCK/WRLCK/UNLCK; same-pid overlaps replace;
 * cross-pid write conflicts yield (block) or EAGAIN (nonblock).
 * SMP-safe via ticket-free spinlock; pure C11 dual-license product path.
 */
#include <gj/file_lock.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/spinlock.h>
#include <gj/string.h>
#include <gj/thread.h>

#define GJ_FLOCK_MAX 32

/* Exclusive end sentinel: -1 means "to EOF" (infinity). */
#define GJ_FLOCK_EOF (-1LL)

struct flock_slot {
    u8  u8Used;
    u8  u8Pad[3];
    i64 i64Fd;
    i16 i16Type;
    i16 i16Pad;
    i64 i64Start;
    i64 i64End; /* exclusive; GJ_FLOCK_EOF = infinity */
    u32 u32Pid;
};

static struct flock_slot g_aLk[GJ_FLOCK_MAX];
static u32               g_u32NLocks;
static struct gj_spinlock g_lkLock;

void
file_lock_init(void)
{
    memset(g_aLk, 0, sizeof(g_aLk));
    g_u32NLocks = 0;
    gj_spin_init(&g_lkLock);
    kprintf("file_lock: init slots=%u locks=0\n", GJ_FLOCK_MAX);
}

static i64
range_end(i64 i64Start, i64 i64Len)
{
    if (i64Len == 0) {
        return GJ_FLOCK_EOF;
    }
    return i64Start + i64Len;
}

static int
ranges_overlap(i64 i64A0, i64 i64A1, i64 i64B0, i64 i64B1)
{
    /* -1 end means infinity — clamp to max positive for the test. */
    if (i64A1 < 0) {
        i64A1 = 0x7fffffffffffffffLL;
    }
    if (i64B1 < 0) {
        i64B1 = 0x7fffffffffffffffLL;
    }
    return i64A0 < i64B1 && i64B0 < i64A1;
}

/**
 * Find a conflicting lock held by another pid on the same fd.
 * Read-read is non-conflicting; any write pair conflicts.
 */
static int
conflict(i64 i64Fd, i16 i16Type, i64 i64Start, i64 i64End, u32 u32SelfPid,
         struct flock_slot **ppHit)
{
    u32 iSlot;

    if (ppHit != NULL) {
        *ppHit = NULL;
    }
    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (!g_aLk[iSlot].u8Used || g_aLk[iSlot].i64Fd != i64Fd) {
            continue;
        }
        if (g_aLk[iSlot].u32Pid == u32SelfPid) {
            continue; /* same pid upgrades/replaces later */
        }
        if (!ranges_overlap(i64Start, i64End, g_aLk[iSlot].i64Start,
                            g_aLk[iSlot].i64End)) {
            continue;
        }
        if (i16Type == GJ_F_RDLCK && g_aLk[iSlot].i16Type == GJ_F_RDLCK) {
            continue;
        }
        if (ppHit != NULL) {
            *ppHit = &g_aLk[iSlot];
        }
        return 1;
    }
    return 0;
}

static void
clear_pid_range(i64 i64Fd, u32 u32Pid, i64 i64Start, i64 i64End)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (!g_aLk[iSlot].u8Used || g_aLk[iSlot].i64Fd != i64Fd ||
            g_aLk[iSlot].u32Pid != u32Pid) {
            continue;
        }
        if (ranges_overlap(i64Start, i64End, g_aLk[iSlot].i64Start,
                           g_aLk[iSlot].i64End)) {
            g_aLk[iSlot].u8Used = 0;
            if (g_u32NLocks > 0) {
                g_u32NLocks--;
            }
        }
    }
}

static i64
flock_validate(i64 i64Fd, const struct gj_flock *pFl, i64 *pi64Start,
               i64 *pi64End, u32 *pu32Pid)
{
    i64 i64Start;
    i64 i64End;
    u32 u32Pid;

    if (pFl == NULL || pi64Start == NULL || pi64End == NULL ||
        pu32Pid == NULL) {
        return -LINUX_EINVAL;
    }
    /* Advisory locks are keyed by fd; reject only clearly invalid fds. */
    if (i64Fd < 0) {
        return -LINUX_EBADF;
    }
    if (pFl->i16Whence != GJ_SEEK_SET) {
        return -LINUX_EINVAL;
    }
    u32Pid = pFl->u32Pid ? pFl->u32Pid : 1u;
    i64Start = pFl->i64Start;
    if (i64Start < 0) {
        i64Start = 0;
    }
    if (pFl->i64Len < 0) {
        return -LINUX_EINVAL;
    }
    i64End = range_end(i64Start, pFl->i64Len);
    *pi64Start = i64Start;
    *pi64End = i64End;
    *pu32Pid = u32Pid;
    return 0;
}

i64
file_lock_set(i64 i64Fd, const struct gj_flock *pFl, int fBlock)
{
    i64 i64Start;
    i64 i64End;
    u32 u32Pid;
    u32 iSlot;
    i64 i64St;

    i64St = flock_validate(i64Fd, pFl, &i64Start, &i64End, &u32Pid);
    if (i64St != 0) {
        return i64St;
    }

    if (pFl->i16Type == GJ_F_UNLCK) {
        gj_spin_lock(&g_lkLock);
        clear_pid_range(i64Fd, u32Pid, i64Start, i64End);
        gj_spin_unlock(&g_lkLock);
        return 0;
    }
    if (pFl->i16Type != GJ_F_RDLCK && pFl->i16Type != GJ_F_WRLCK) {
        return -LINUX_EINVAL;
    }

    for (;;) {
        gj_spin_lock(&g_lkLock);
        if (conflict(i64Fd, pFl->i16Type, i64Start, i64End, u32Pid, NULL)) {
            gj_spin_unlock(&g_lkLock);
            if (!fBlock) {
                return -LINUX_EAGAIN;
            }
            /* F_SETLKW: cooperative yield until the conflicting range frees. */
            thread_yield();
            continue;
        }

        /* Replace any overlapping locks owned by this pid. */
        clear_pid_range(i64Fd, u32Pid, i64Start, i64End);

        for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
            if (!g_aLk[iSlot].u8Used) {
                g_aLk[iSlot].u8Used = 1;
                g_aLk[iSlot].i64Fd = i64Fd;
                g_aLk[iSlot].i16Type = pFl->i16Type;
                g_aLk[iSlot].i64Start = i64Start;
                g_aLk[iSlot].i64End = i64End;
                g_aLk[iSlot].u32Pid = u32Pid;
                g_u32NLocks++;
                gj_spin_unlock(&g_lkLock);
                return 0;
            }
        }
        gj_spin_unlock(&g_lkLock);
        return -LINUX_ENOLCK;
    }
}

i64
file_lock_get(i64 i64Fd, struct gj_flock *pFlInOut)
{
    i64 i64Start;
    i64 i64End;
    u32 u32Pid;
    i64 i64St;
    i16 i16Probe;
    struct flock_slot *pHit = NULL;

    i64St = flock_validate(i64Fd, pFlInOut, &i64Start, &i64End, &u32Pid);
    if (i64St != 0) {
        return i64St;
    }

    /* UNLCK probe → treat as write so any lock is reported (POSIX F_GETLK). */
    i16Probe = (pFlInOut->i16Type == GJ_F_UNLCK) ? GJ_F_WRLCK
                                                  : pFlInOut->i16Type;
    if (i16Probe != GJ_F_RDLCK && i16Probe != GJ_F_WRLCK) {
        return -LINUX_EINVAL;
    }

    gj_spin_lock(&g_lkLock);
    if (conflict(i64Fd, i16Probe, i64Start, i64End, u32Pid, &pHit) &&
        pHit != NULL) {
        pFlInOut->i16Type = pHit->i16Type;
        pFlInOut->i16Whence = GJ_SEEK_SET;
        pFlInOut->i64Start = pHit->i64Start;
        pFlInOut->i64Len =
            pHit->i64End < 0 ? 0 : (pHit->i64End - pHit->i64Start);
        pFlInOut->u32Pid = pHit->u32Pid;
        gj_spin_unlock(&g_lkLock);
        return 0;
    }
    gj_spin_unlock(&g_lkLock);
    pFlInOut->i16Type = GJ_F_UNLCK;
    pFlInOut->i16Whence = GJ_SEEK_SET;
    return 0;
}

u32
file_lock_count(void)
{
    u32 u32N;

    gj_spin_lock(&g_lkLock);
    u32N = g_u32NLocks;
    gj_spin_unlock(&g_lkLock);
    return u32N;
}

u32
file_lock_release_fd(i64 i64Fd)
{
    u32 iSlot;
    u32 u32Cleared = 0;

    if (i64Fd < 0) {
        return 0;
    }
    gj_spin_lock(&g_lkLock);
    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (g_aLk[iSlot].u8Used && g_aLk[iSlot].i64Fd == i64Fd) {
            g_aLk[iSlot].u8Used = 0;
            if (g_u32NLocks > 0) {
                g_u32NLocks--;
            }
            u32Cleared++;
        }
    }
    gj_spin_unlock(&g_lkLock);
    return u32Cleared;
}

u32
file_lock_release_pid(u32 u32Pid)
{
    u32 iSlot;
    u32 u32Cleared = 0;

    if (u32Pid == 0) {
        return 0;
    }
    gj_spin_lock(&g_lkLock);
    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (g_aLk[iSlot].u8Used && g_aLk[iSlot].u32Pid == u32Pid) {
            g_aLk[iSlot].u8Used = 0;
            if (g_u32NLocks > 0) {
                g_u32NLocks--;
            }
            u32Cleared++;
        }
    }
    gj_spin_unlock(&g_lkLock);
    return u32Cleared;
}
