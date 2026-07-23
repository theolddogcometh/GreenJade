/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Advisory byte-range locks for wine/Proton file locking.
 * Per-fd ranges with RDLCK/WRLCK/UNLCK; same-pid overlaps carve/split;
 * cross-pid write conflicts soft-block (F_SETLKW) or EAGAIN (nonblock).
 *
 * Deepen (v1, exclusive file track):
 *   - FLOCK_CONFLICT_SPLIT: partial UNLCK/SETLK leaves non-overlapped remnants
 *   - FLOCK_SOFT_MULTI_WAITER: register waiters, thread_block/wake, gen bump
 *   - Soft deadlock: short cycle over waiter table → -GJ_FLOCK_EDEADLK
 *   - GETLK: lowest-start conflict; write preferred on equal start
 *   - Overflow-safe range ends; adjacent same-type coalesce
 *
 * Soft lock inventory (Wave 8 exclusive deepen):
 *   - Live held + free slot counts; peak held; waiter / wake_gen snapshot
 *   - Soft deny tallies: EAGAIN (nonblock conflict), EDEADLK, ENOLCK
 *   - Cumulative set_ok / unlk_ok / get / get_hit / block / wake / release
 *   greppable: "file_lock: soft …"
 *
 * SMP-safe via ticket-free spinlock; pure C11 dual-license product path.
 */
#include <gj/file_lock.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/spinlock.h>
#include <gj/string.h>
#include <gj/thread.h>

#define GJ_FLOCK_MAX         32
#define GJ_FLOCK_MAX_WAITERS 8
/* Soft multi-wake budget per unlock/release (bounded; not a hard queue). */
#define GJ_FLOCK_SOFT_WAKE_MAX 4u
#define GJ_FLOCK_TAG_WAITER    1u
/* Soft deadlock chain walk depth (pid edges via waiter table). */
#define GJ_FLOCK_DEADLOCK_DEPTH 4u

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

/*
 * Soft F_SETLKW waiter — non-exclusive registration for cycle detect + stats.
 * greppable: FLOCK_SOFT_MULTI_WAITER
 */
struct flock_waiter {
    u8  u8Used;
    u8  u8Pad[3];
    i64 i64Fd;
    i16 i16Type;
    i16 i16Pad;
    i64 i64Start;
    i64 i64End;
    u32 u32Pid;
    u32 u32BlockerPid; /* last observed conflicting holder */
    u32 u32GenSeen;
};

static struct flock_slot   g_aLk[GJ_FLOCK_MAX];
static struct flock_waiter g_aWait[GJ_FLOCK_MAX_WAITERS];
static u32                 g_u32NLocks;
static u32                 g_u32NWaiters;
static u32                 g_u32WakeGen;
static struct gj_spinlock  g_lkLock;
/* Soft multi-waiter block object (all fcntl waiters; re-check on wake). */
static u32                 g_u32WaitObj;

/*
 * Soft product inventory (Wave 8). Cumulative unless noted live/peak.
 * greppable: file_lock: soft …
 */
static u32 g_u32SoftHeldPeak;   /* max live held (g_u32NLocks high-water) */
static u32 g_u32SoftSetOk;      /* successful RDLCK/WRLCK grants */
static u32 g_u32SoftUnlkOk;     /* successful UNLCK carves */
static u32 g_u32SoftDeny;       /* soft denies total (eagain+deadlk+nolck) */
static u32 g_u32SoftDenyEagain; /* nonblock conflict → -EAGAIN */
static u32 g_u32SoftDenyDeadlk; /* soft cycle → -EDEADLK */
static u32 g_u32SoftDenyNolck;  /* table full / carve fail → -ENOLCK */
static u32 g_u32SoftGet;        /* F_GETLK probes */
static u32 g_u32SoftGetHit;     /* GETLK conflict rewrites */
static u32 g_u32SoftBlock;      /* times entered F_SETLKW wait path */
static u32 g_u32SoftWakeCalls;  /* soft multi-wake invocations */
static u32 g_u32SoftWakeN;      /* waiters woken (sum of thread_wake ret) */
static u32 g_u32SoftRelFd;      /* slots cleared via release_fd */
static u32 g_u32SoftRelPid;     /* slots cleared via release_pid */

static void soft_held_note(void);
static void soft_deny_note(u32 *pu32Bucket);
static void soft_inventory_log(void);

void
file_lock_init(void)
{
    memset(g_aLk, 0, sizeof(g_aLk));
    memset(g_aWait, 0, sizeof(g_aWait));
    g_u32NLocks = 0;
    g_u32NWaiters = 0;
    g_u32WakeGen = 0;
    g_u32WaitObj = 0;
    g_u32SoftHeldPeak = 0;
    g_u32SoftSetOk = 0;
    g_u32SoftUnlkOk = 0;
    g_u32SoftDeny = 0;
    g_u32SoftDenyEagain = 0;
    g_u32SoftDenyDeadlk = 0;
    g_u32SoftDenyNolck = 0;
    g_u32SoftGet = 0;
    g_u32SoftGetHit = 0;
    g_u32SoftBlock = 0;
    g_u32SoftWakeCalls = 0;
    g_u32SoftWakeN = 0;
    g_u32SoftRelFd = 0;
    g_u32SoftRelPid = 0;
    gj_spin_init(&g_lkLock);
    kprintf("file_lock: init slots=%u waiters=%u locks=0\n", GJ_FLOCK_MAX,
            GJ_FLOCK_MAX_WAITERS);
    /* Grep: file_lock: soft (baseline inventory after init) */
    soft_inventory_log();
}

/**
 * Note live held high-water. Caller holds g_lkLock (or single-threaded init).
 */
static void
soft_held_note(void)
{
    if (g_u32NLocks > g_u32SoftHeldPeak) {
        g_u32SoftHeldPeak = g_u32NLocks;
    }
}

/**
 * Bump soft deny total + optional typed bucket. Caller holds g_lkLock or
 * is on a terminal return path before unlock is fine either way (u32 only).
 */
static void
soft_deny_note(u32 *pu32Bucket)
{
    g_u32SoftDeny++;
    if (pu32Bucket != NULL) {
        (*pu32Bucket)++;
    }
}

/**
 * Greppable soft lock inventory (product / smoke).
 *   file_lock: soft held=… free=… peak=… waiters=… gen=…
 *   file_lock: soft deny=… eagain=… deadlk=… nolck=… set_ok=… unlk_ok=…
 *   file_lock: soft get=… get_hit=… block=… wake=… wake_n=… rel_fd=… rel_pid=…
 */
static void
soft_inventory_log(void)
{
    u32 u32Held;
    u32 u32Wait;
    u32 u32Gen;
    u32 u32Free;
    u32 u32Peak;
    u32 u32Deny;
    u32 u32Eagain;
    u32 u32Deadlk;
    u32 u32Nolck;
    u32 u32SetOk;
    u32 u32UnlkOk;
    u32 u32Get;
    u32 u32GetHit;
    u32 u32Block;
    u32 u32Wake;
    u32 u32WakeN;
    u32 u32RelFd;
    u32 u32RelPid;

    gj_spin_lock(&g_lkLock);
    u32Held = g_u32NLocks;
    u32Wait = g_u32NWaiters;
    u32Gen = g_u32WakeGen;
    /* Bound free so soft inventory never wraps if counters ever desync. */
    u32Free = (u32Held < (u32)GJ_FLOCK_MAX) ? ((u32)GJ_FLOCK_MAX - u32Held)
                                            : 0u;
    u32Peak = g_u32SoftHeldPeak;
    u32Deny = g_u32SoftDeny;
    u32Eagain = g_u32SoftDenyEagain;
    u32Deadlk = g_u32SoftDenyDeadlk;
    u32Nolck = g_u32SoftDenyNolck;
    u32SetOk = g_u32SoftSetOk;
    u32UnlkOk = g_u32SoftUnlkOk;
    u32Get = g_u32SoftGet;
    u32GetHit = g_u32SoftGetHit;
    u32Block = g_u32SoftBlock;
    u32Wake = g_u32SoftWakeCalls;
    u32WakeN = g_u32SoftWakeN;
    u32RelFd = g_u32SoftRelFd;
    u32RelPid = g_u32SoftRelPid;
    gj_spin_unlock(&g_lkLock);

    kprintf("file_lock: soft held=%u free=%u peak=%u waiters=%u gen=%u\n",
            u32Held, u32Free, u32Peak, u32Wait, u32Gen);
    kprintf("file_lock: soft deny=%u eagain=%u deadlk=%u nolck=%u "
            "set_ok=%u unlk_ok=%u\n",
            u32Deny, u32Eagain, u32Deadlk, u32Nolck, u32SetOk, u32UnlkOk);
    kprintf("file_lock: soft get=%u get_hit=%u block=%u wake=%u wake_n=%u "
            "rel_fd=%u rel_pid=%u\n",
            u32Get, u32GetHit, u32Block, u32Wake, u32WakeN, u32RelFd,
            u32RelPid);
}

static i64
range_end(i64 i64Start, i64 i64Len)
{
    if (i64Len == 0) {
        return GJ_FLOCK_EOF;
    }
    /* Overflow-safe: start+len must stay in positive i64 range. */
    if (i64Start > 0x7fffffffffffffffLL - i64Len) {
        return GJ_FLOCK_EOF; /* treated as invalid by validate */
    }
    return i64Start + i64Len;
}

static int
range_overflow(i64 i64Start, i64 i64Len)
{
    if (i64Len == 0) {
        return 0;
    }
    return i64Start > 0x7fffffffffffffffLL - i64Len;
}

static i64
range_norm_end(i64 i64End)
{
    if (i64End < 0) {
        return 0x7fffffffffffffffLL;
    }
    return i64End;
}

static int
ranges_overlap(i64 i64A0, i64 i64A1, i64 i64B0, i64 i64B1)
{
    i64A1 = range_norm_end(i64A1);
    i64B1 = range_norm_end(i64B1);
    return i64A0 < i64B1 && i64B0 < i64A1;
}

/**
 * True if two lock types conflict (POSIX): any write vs any lock.
 * Read-read is non-conflicting.
 */
static int
types_conflict(i16 i16A, i16 i16B)
{
    if (i16A == GJ_F_RDLCK && i16B == GJ_F_RDLCK) {
        return 0;
    }
    return 1;
}

static int
slot_free_count(void)
{
    u32 iSlot;
    int nFree = 0;

    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (!g_aLk[iSlot].u8Used) {
            nFree++;
        }
    }
    return nFree;
}

static struct flock_slot *
slot_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (!g_aLk[iSlot].u8Used) {
            g_aLk[iSlot].u8Used = 1;
            g_u32NLocks++;
            soft_held_note(); /* live held high-water (soft inventory) */
            return &g_aLk[iSlot];
        }
    }
    return NULL;
}

static void
slot_free(struct flock_slot *pS)
{
    if (pS == NULL || !pS->u8Used) {
        return;
    }
    pS->u8Used = 0;
    if (g_u32NLocks > 0) {
        g_u32NLocks--;
    }
    /* Peak is never reduced; live held falls with free. */
}

static void
slot_fill(struct flock_slot *pS, i64 i64Fd, i16 i16Type, i64 i64Start,
          i64 i64End, u32 u32Pid)
{
    pS->u8Used = 1;
    pS->i64Fd = i64Fd;
    pS->i16Type = i16Type;
    pS->i64Start = i64Start;
    pS->i64End = i64End;
    pS->u32Pid = u32Pid;
}

/**
 * Soft wake generation bump + multi-wake of F_SETLKW waiters.
 * Caller holds g_lkLock for the gen store; wake is after unlock.
 * greppable: FLOCK_SOFT_MULTI_WAKE
 */
static void
wake_gen_bump(void)
{
    g_u32WakeGen++;
}

static u32
soft_wake_waiters(void)
{
    u32 u32N;

    /* All soft waiters share g_u32WaitObj; they re-check conflict. */
    u32N = thread_wake(&g_u32WaitObj, GJ_FLOCK_TAG_WAITER, GJ_FLOCK_SOFT_WAKE_MAX);
    g_u32SoftWakeCalls++;
    g_u32SoftWakeN += u32N;
    return u32N;
}

/**
 * Find a conflicting lock held by another pid on the same fd.
 * Among hits: lowest start wins; write preferred when starts equal.
 * greppable: FLOCK_CONFLICT_DETECT
 */
static int
conflict(i64 i64Fd, i16 i16Type, i64 i64Start, i64 i64End, u32 u32SelfPid,
         struct flock_slot **ppHit)
{
    u32 iSlot;
    struct flock_slot *pBest = NULL;

    if (ppHit != NULL) {
        *ppHit = NULL;
    }
    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        struct flock_slot *pS = &g_aLk[iSlot];

        if (!pS->u8Used || pS->i64Fd != i64Fd) {
            continue;
        }
        if (pS->u32Pid == u32SelfPid) {
            continue; /* same pid upgrades/replaces later */
        }
        if (!ranges_overlap(i64Start, i64End, pS->i64Start, pS->i64End)) {
            continue;
        }
        if (!types_conflict(i16Type, pS->i16Type)) {
            continue;
        }
        if (pBest == NULL) {
            pBest = pS;
            continue;
        }
        /* Prefer lower start; on tie prefer write (more informative GETLK). */
        if (pS->i64Start < pBest->i64Start) {
            pBest = pS;
        } else if (pS->i64Start == pBest->i64Start &&
                   pS->i16Type == GJ_F_WRLCK &&
                   pBest->i16Type != GJ_F_WRLCK) {
            pBest = pS;
        }
    }
    if (pBest != NULL) {
        if (ppHit != NULL) {
            *ppHit = pBest;
        }
        return 1;
    }
    return 0;
}

/**
 * Carve [i64Start,i64End) out of own locks on i64Fd/u32Pid.
 * Leaves non-overlapped remnants (split). Returns 0 or -LINUX_ENOLCK.
 * greppable: FLOCK_CONFLICT_SPLIT
 */
static i64
carve_pid_range(i64 i64Fd, u32 u32Pid, i64 i64Start, i64 i64End)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        struct flock_slot *pS = &g_aLk[iSlot];
        i64 i64L0;
        i64 i64L1;
        i64 i64U0;
        i64 i64U1;
        i16 i16Type;
        int fLeft;
        int fRight;

        if (!pS->u8Used || pS->i64Fd != i64Fd || pS->u32Pid != u32Pid) {
            continue;
        }
        if (!ranges_overlap(i64Start, i64End, pS->i64Start, pS->i64End)) {
            continue;
        }

        i64L0 = pS->i64Start;
        i64L1 = pS->i64End;
        i64U0 = i64Start;
        i64U1 = i64End;
        i16Type = pS->i16Type;

        /* Normalize infinity only for edge tests; store keeps EOF sentinel. */
        fLeft = i64L0 < i64U0;
        fRight = range_norm_end(i64L1) > range_norm_end(i64U1);

        if (!fLeft && !fRight) {
            /* Fully covered — drop. */
            slot_free(pS);
            continue;
        }
        if (fLeft && fRight) {
            /* Hole: need a free slot for the right remnant. */
            struct flock_slot *pR;

            if (slot_free_count() < 1) {
                return -LINUX_ENOLCK;
            }
            /* Left remnant in place. */
            pS->i64End = i64U0;
            pR = slot_alloc();
            if (pR == NULL) {
                /* Restore original end on failure. */
                pS->i64End = i64L1;
                return -LINUX_ENOLCK;
            }
            slot_fill(pR, i64Fd, i16Type, i64U1, i64L1, u32Pid);
            continue;
        }
        if (fLeft) {
            /* Shrink from right: [L0, U0) */
            pS->i64End = i64U0;
            continue;
        }
        /* fRight only: shrink from left [U1, L1) */
        pS->i64Start = i64U1;
    }
    return 0;
}

/**
 * Coalesce adjacent/overlapping same-pid same-type ranges on fd.
 * Soft merge after SETLK insert. greppable: FLOCK_COALESCE
 */
static void
coalesce_pid(i64 i64Fd, u32 u32Pid)
{
    u32 iA;
    u32 iB;

    for (iA = 0; iA < GJ_FLOCK_MAX; iA++) {
        struct flock_slot *pA = &g_aLk[iA];

        if (!pA->u8Used || pA->i64Fd != i64Fd || pA->u32Pid != u32Pid) {
            continue;
        }
        for (iB = iA + 1; iB < GJ_FLOCK_MAX; iB++) {
            struct flock_slot *pB = &g_aLk[iB];
            i64 i64A0;
            i64 i64A1;
            i64 i64B0;
            i64 i64B1;
            i64 i64N0;
            i64 i64N1;

            if (!pB->u8Used || pB->i64Fd != i64Fd || pB->u32Pid != u32Pid) {
                continue;
            }
            if (pA->i16Type != pB->i16Type) {
                continue;
            }
            i64A0 = pA->i64Start;
            i64A1 = range_norm_end(pA->i64End);
            i64B0 = pB->i64Start;
            i64B1 = range_norm_end(pB->i64End);
            /* Adjacent or overlapping. */
            if (i64A1 < i64B0 || i64B1 < i64A0) {
                continue;
            }
            i64N0 = i64A0 < i64B0 ? i64A0 : i64B0;
            if (pA->i64End < 0 || pB->i64End < 0) {
                i64N1 = GJ_FLOCK_EOF;
            } else {
                i64N1 = i64A1 > i64B1 ? i64A1 : i64B1;
            }
            pA->i64Start = i64N0;
            pA->i64End = i64N1;
            slot_free(pB);
        }
    }
}

/**
 * Soft waiter register. Returns waiter slot index or -1 if table full
 * (still allowed to yield-spin without registration).
 */
static int
waiter_register(i64 i64Fd, i16 i16Type, i64 i64Start, i64 i64End, u32 u32Pid,
                u32 u32BlockerPid)
{
    u32 iW;

    for (iW = 0; iW < GJ_FLOCK_MAX_WAITERS; iW++) {
        if (!g_aWait[iW].u8Used) {
            g_aWait[iW].u8Used = 1;
            g_aWait[iW].i64Fd = i64Fd;
            g_aWait[iW].i16Type = i16Type;
            g_aWait[iW].i64Start = i64Start;
            g_aWait[iW].i64End = i64End;
            g_aWait[iW].u32Pid = u32Pid;
            g_aWait[iW].u32BlockerPid = u32BlockerPid;
            g_aWait[iW].u32GenSeen = g_u32WakeGen;
            g_u32NWaiters++;
            return (int)iW;
        }
    }
    return -1;
}

static void
waiter_unregister(int nIdx)
{
    if (nIdx < 0 || nIdx >= GJ_FLOCK_MAX_WAITERS) {
        return;
    }
    if (!g_aWait[nIdx].u8Used) {
        return;
    }
    g_aWait[nIdx].u8Used = 0;
    if (g_u32NWaiters > 0) {
        g_u32NWaiters--;
    }
}

static void
waiter_update_blocker(int nIdx, u32 u32BlockerPid)
{
    if (nIdx < 0 || nIdx >= GJ_FLOCK_MAX_WAITERS) {
        return;
    }
    if (!g_aWait[nIdx].u8Used) {
        return;
    }
    g_aWait[nIdx].u32BlockerPid = u32BlockerPid;
    g_aWait[nIdx].u32GenSeen = g_u32WakeGen;
}

/**
 * Soft deadlock: follow u32BlockerPid edges among waiters.
 * If u32SelfPid appears in the chain, a cycle exists.
 * greppable: FLOCK_SOFT_DEADLOCK
 */
static int
soft_deadlock(u32 u32SelfPid, u32 u32BlockerPid)
{
    u32 u32Cur;
    u32 u32Depth;
    u32 iW;

    if (u32BlockerPid == 0 || u32SelfPid == 0) {
        return 0;
    }
    if (u32BlockerPid == u32SelfPid) {
        return 1;
    }
    u32Cur = u32BlockerPid;
    for (u32Depth = 0; u32Depth < GJ_FLOCK_DEADLOCK_DEPTH; u32Depth++) {
        u32 u32Next = 0;
        int fFound = 0;

        for (iW = 0; iW < GJ_FLOCK_MAX_WAITERS; iW++) {
            if (!g_aWait[iW].u8Used || g_aWait[iW].u32Pid != u32Cur) {
                continue;
            }
            u32Next = g_aWait[iW].u32BlockerPid;
            fFound = 1;
            break;
        }
        if (!fFound || u32Next == 0) {
            return 0;
        }
        if (u32Next == u32SelfPid) {
            return 1;
        }
        u32Cur = u32Next;
    }
    return 0;
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
    if (range_overflow(i64Start, pFl->i64Len)) {
        return -LINUX_EINVAL;
    }
    i64End = range_end(i64Start, pFl->i64Len);
    /* Empty finite range (start==end with len>0 impossible after end calc). */
    if (pFl->i64Len > 0 && i64End == i64Start) {
        return -LINUX_EINVAL;
    }
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
    i64 i64St;
    int nWait = -1;
    int fRegistered = 0;
    struct gj_thread *pCur;

    i64St = flock_validate(i64Fd, pFl, &i64Start, &i64End, &u32Pid);
    if (i64St != 0) {
        return i64St;
    }

    if (pFl->i16Type == GJ_F_UNLCK) {
        gj_spin_lock(&g_lkLock);
        i64St = carve_pid_range(i64Fd, u32Pid, i64Start, i64End);
        if (i64St == 0) {
            wake_gen_bump();
            g_u32SoftUnlkOk++;
        } else if (i64St == -LINUX_ENOLCK) {
            soft_deny_note(&g_u32SoftDenyNolck);
        }
        gj_spin_unlock(&g_lkLock);
        if (i64St == 0) {
            (void)soft_wake_waiters();
        }
        return i64St;
    }
    if (pFl->i16Type != GJ_F_RDLCK && pFl->i16Type != GJ_F_WRLCK) {
        return -LINUX_EINVAL;
    }

    pCur = thread_current();

    for (;;) {
        struct flock_slot *pHit = NULL;
        struct flock_slot *pNew;
        u32 u32Blocker = 0;

        gj_spin_lock(&g_lkLock);
        if (conflict(i64Fd, pFl->i16Type, i64Start, i64End, u32Pid, &pHit)) {
            if (pHit != NULL) {
                u32Blocker = pHit->u32Pid;
            }
            if (!fBlock) {
                soft_deny_note(&g_u32SoftDenyEagain);
                gj_spin_unlock(&g_lkLock);
                if (fRegistered) {
                    gj_spin_lock(&g_lkLock);
                    waiter_unregister(nWait);
                    gj_spin_unlock(&g_lkLock);
                }
                return -LINUX_EAGAIN;
            }
            /* Soft deadlock before sleeping. greppable: FLOCK_SOFT_DEADLOCK */
            if (soft_deadlock(u32Pid, u32Blocker)) {
                soft_deny_note(&g_u32SoftDenyDeadlk);
                if (fRegistered) {
                    waiter_unregister(nWait);
                }
                gj_spin_unlock(&g_lkLock);
                return -(i64)GJ_FLOCK_EDEADLK;
            }
            g_u32SoftBlock++;
            if (!fRegistered) {
                nWait = waiter_register(i64Fd, pFl->i16Type, i64Start, i64End,
                                        u32Pid, u32Blocker);
                fRegistered = (nWait >= 0) ? 1 : 0;
            } else {
                waiter_update_blocker(nWait, u32Blocker);
            }
            gj_spin_unlock(&g_lkLock);

            /*
             * F_SETLKW soft multi-waiter: block when a thread context exists;
             * otherwise cooperative yield (early boot / no thr).
             * greppable: FLOCK_SOFT_MULTI_WAITER
             *
             * Lost-wake: unlock may soft-wake between the conflict sample and
             * BLOCKED. Re-sample under lock after thread_block; if free (or
             * wake gen moved), self soft-wake so schedule() does not hang.
             */
            if (pCur != NULL) {
                thread_block(&g_u32WaitObj, GJ_FLOCK_TAG_WAITER);
                gj_spin_lock(&g_lkLock);
                if (!conflict(i64Fd, pFl->i16Type, i64Start, i64End, u32Pid,
                              NULL)) {
                    gj_spin_unlock(&g_lkLock);
                    (void)soft_wake_waiters();
                } else {
                    gj_spin_unlock(&g_lkLock);
                }
                schedule();
            } else {
                thread_yield();
            }
            continue;
        }

        /* Carve own overlapping ranges, then insert. */
        i64St = carve_pid_range(i64Fd, u32Pid, i64Start, i64End);
        if (i64St != 0) {
            if (i64St == -LINUX_ENOLCK) {
                soft_deny_note(&g_u32SoftDenyNolck);
            }
            if (fRegistered) {
                waiter_unregister(nWait);
            }
            gj_spin_unlock(&g_lkLock);
            return i64St;
        }
        pNew = slot_alloc();
        if (pNew == NULL) {
            soft_deny_note(&g_u32SoftDenyNolck);
            if (fRegistered) {
                waiter_unregister(nWait);
            }
            gj_spin_unlock(&g_lkLock);
            return -LINUX_ENOLCK;
        }
        slot_fill(pNew, i64Fd, pFl->i16Type, i64Start, i64End, u32Pid);
        coalesce_pid(i64Fd, u32Pid);
        g_u32SoftSetOk++;
        if (fRegistered) {
            waiter_unregister(nWait);
        }
        /* SET may upgrade read→write and free readers' soft wait paths. */
        wake_gen_bump();
        gj_spin_unlock(&g_lkLock);
        (void)soft_wake_waiters();
        return 0;
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
    g_u32SoftGet++;
    if (conflict(i64Fd, i16Probe, i64Start, i64End, u32Pid, &pHit) &&
        pHit != NULL) {
        pFlInOut->i16Type = pHit->i16Type;
        pFlInOut->i16Whence = GJ_SEEK_SET;
        pFlInOut->i64Start = pHit->i64Start;
        pFlInOut->i64Len =
            pHit->i64End < 0 ? 0 : (pHit->i64End - pHit->i64Start);
        pFlInOut->u32Pid = pHit->u32Pid;
        g_u32SoftGetHit++;
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
    /*
     * Emit soft inventory on stats read so bring-up smoke
     * (file_lock: count=… PASS) also greps held/deny lines.
     * greppable: file_lock: soft
     */
    soft_inventory_log();
    return u32N;
}

u32
file_lock_waiters(void)
{
    u32 u32N;

    gj_spin_lock(&g_lkLock);
    u32N = g_u32NWaiters;
    gj_spin_unlock(&g_lkLock);
    return u32N;
}

u32
file_lock_wake_gen(void)
{
    u32 u32G;

    gj_spin_lock(&g_lkLock);
    u32G = g_u32WakeGen;
    gj_spin_unlock(&g_lkLock);
    return u32G;
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
            slot_free(&g_aLk[iSlot]);
            u32Cleared++;
        }
    }
    if (u32Cleared > 0) {
        wake_gen_bump();
        g_u32SoftRelFd += u32Cleared;
    }
    gj_spin_unlock(&g_lkLock);
    if (u32Cleared > 0) {
        (void)soft_wake_waiters();
    }
    return u32Cleared;
}

u32
file_lock_release_pid(u32 u32Pid)
{
    u32 iSlot;
    u32 u32Cleared = 0;
    u32 iW;

    if (u32Pid == 0) {
        return 0;
    }
    gj_spin_lock(&g_lkLock);
    for (iSlot = 0; iSlot < GJ_FLOCK_MAX; iSlot++) {
        if (g_aLk[iSlot].u8Used && g_aLk[iSlot].u32Pid == u32Pid) {
            slot_free(&g_aLk[iSlot]);
            u32Cleared++;
        }
    }
    /* Drop soft waiters owned by the exiting pid (no orphan cycle edges). */
    for (iW = 0; iW < GJ_FLOCK_MAX_WAITERS; iW++) {
        if (g_aWait[iW].u8Used && g_aWait[iW].u32Pid == u32Pid) {
            g_aWait[iW].u8Used = 0;
            if (g_u32NWaiters > 0) {
                g_u32NWaiters--;
            }
        }
    }
    if (u32Cleared > 0) {
        wake_gen_bump();
        g_u32SoftRelPid += u32Cleared;
    }
    gj_spin_unlock(&g_lkLock);
    if (u32Cleared > 0) {
        (void)soft_wake_waiters();
    }
    return u32Cleared;
}
