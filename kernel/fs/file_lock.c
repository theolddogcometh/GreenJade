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
 * Soft lock inventory (Wave 8 baseline + Wave 13 deepen + Wave 19 exclusive):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Live held + free slot counts; peak held; waiter / wake_gen snapshot
 *   - Soft deny tallies: EAGAIN (nonblock conflict), EDEADLK, ENOLCK
 *   - Cumulative set_ok / unlk_ok / get / get_hit / block / wake / release
 * Wave 13 exclusive (this unit only — greppable "file_lock: soft …"):
 *   - Path enter/grant split: set rd|wr, unlk, validate/type reject
 *   - Waiter reg/full/unreg + thr-block vs yield + lost-wake self
 *   - Carve/split/coalesce + conflict sample + release call/zero/wait-drop
 *   - Live type scan (rd/wr/eof) under lock on inventory dump
 *   - Sub-lines: inventory|held|deny|set|get|block|wake|wait|carve|
 *                release|types|path (primary held/deny/get field-stable)
 * Wave 19 exclusive (this unit only — greppable "file_lock: soft …"):
 *   - Waiter peak; deadlk probe/hit; eof|finite grants; wake zero|some
 *   - Complementary: total|rate|deadlk|grant|catalog|deepen
 *   Soft honesty remains soft: advisory inventory ≠ bar3 / product flock
 *   Never hard-gates; wrap-OK counters. Soft ≠ bar3.
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
 * Soft product inventory (Wave 8 baseline + Wave 13 + Wave 20 deepen).
 * Cumulative unless noted live/peak. Never hard-gates. Wrap OK.
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

/*
 * Wave 13 exclusive path tallies (file-local deepen; greppable "file_lock: soft …").
 * Cumulative; wrap-OK; never reshape set/get product returns.
 */
static u32 g_u32SoftSetEnter;     /* file_lock_set entries */
static u32 g_u32SoftSetRd;        /* RDLCK grants */
static u32 g_u32SoftSetWr;        /* WRLCK grants */
static u32 g_u32SoftUnlkEnter;    /* UNLCK path entries */
static u32 g_u32SoftValFail;      /* flock_validate rejects */
static u32 g_u32SoftTypeInval;    /* bad i16Type on set */
static u32 g_u32SoftGetMiss;      /* GETLK free (type→UNLCK) */
static u32 g_u32SoftGetUnlkProbe; /* GETLK with UNLCK probe type */
static u32 g_u32SoftGetValFail;   /* GETLK validate reject */
static u32 g_u32SoftBlockThr;     /* thread_block F_SETLKW path */
static u32 g_u32SoftBlockYield;   /* yield path (no thr context) */
static u32 g_u32SoftLostWakeSelf; /* self soft-wake after lost-wake */
static u32 g_u32SoftWaitReg;      /* waiter_register success */
static u32 g_u32SoftWaitFull;     /* waiter table full */
static u32 g_u32SoftWaitUnreg;    /* waiter_unregister */
static u32 g_u32SoftCarveOk;      /* carve_pid_range returns 0 */
static u32 g_u32SoftCarveNolck;   /* carve ENOLCK */
static u32 g_u32SoftSplitHole;    /* split hole (left+right remnant) */
static u32 g_u32SoftCoalesce;     /* coalesce freed a sibling slot */
static u32 g_u32SoftConflict;     /* conflict() positive hits */
static u32 g_u32SoftRelFdCall;    /* release_fd entries */
static u32 g_u32SoftRelFdZero;    /* release_fd cleared 0 */
static u32 g_u32SoftRelPidCall;   /* release_pid entries */
static u32 g_u32SoftRelPidZero;   /* release_pid cleared 0 */
static u32 g_u32SoftRelWaitDrop;  /* waiters dropped on release_pid */
static u32 g_u32SoftWakeGenBump;  /* wake_gen_bump calls */
static u32 g_u32SoftCountSample;  /* file_lock_count samples */
static u32 g_u32SoftWaitSample;   /* file_lock_waiters samples */
static u32 g_u32SoftGenSample;    /* file_lock_wake_gen samples */
static u32 g_u32SoftLogN;         /* soft inventory emissions */
static u8  g_fSoftOnce;           /* one-shot after first activity */

/* Wave 35 exclusive soft deepen — complementary path tallies. */
static u32 g_u32SoftWaitPeak;     /* peak live waiters */
static u32 g_u32SoftDeadlkProbe;  /* soft_deadlock probes */
static u32 g_u32SoftDeadlkHit;    /* soft_deadlock true */
static u32 g_u32SoftEofGrant;     /* SET grant with EOF end */
static u32 g_u32SoftFiniteGrant;  /* SET grant with finite end */
static u32 g_u32SoftWakeZero;     /* soft_wake returned 0 */
static u32 g_u32SoftWakeSome;     /* soft_wake returned >0 */
static u32 g_u32SoftBlockRetry;   /* continue after block/yield */
static u32 g_u32SoftLastType;     /* last set type (rd/wr/unlk codes) */
static u32 g_u32SoftLastDenyCode; /* 0 none, 1 eagain, 2 deadlk, 3 nolck */

static void soft_inc(u32 *pu32Ctr);
static void soft_held_note(void);
static void soft_wait_peak_note(void);
static void soft_deny_note(u32 *pu32Bucket);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

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
    /* Wave 13 exclusive path tallies. */
    g_u32SoftSetEnter = 0;
    g_u32SoftSetRd = 0;
    g_u32SoftSetWr = 0;
    g_u32SoftUnlkEnter = 0;
    g_u32SoftValFail = 0;
    g_u32SoftTypeInval = 0;
    g_u32SoftGetMiss = 0;
    g_u32SoftGetUnlkProbe = 0;
    g_u32SoftGetValFail = 0;
    g_u32SoftBlockThr = 0;
    g_u32SoftBlockYield = 0;
    g_u32SoftLostWakeSelf = 0;
    g_u32SoftWaitReg = 0;
    g_u32SoftWaitFull = 0;
    g_u32SoftWaitUnreg = 0;
    g_u32SoftCarveOk = 0;
    g_u32SoftCarveNolck = 0;
    g_u32SoftSplitHole = 0;
    g_u32SoftCoalesce = 0;
    g_u32SoftConflict = 0;
    g_u32SoftRelFdCall = 0;
    g_u32SoftRelFdZero = 0;
    g_u32SoftRelPidCall = 0;
    g_u32SoftRelPidZero = 0;
    g_u32SoftRelWaitDrop = 0;
    g_u32SoftWakeGenBump = 0;
    g_u32SoftCountSample = 0;
    g_u32SoftWaitSample = 0;
    g_u32SoftGenSample = 0;
    g_u32SoftLogN = 0;
    g_fSoftOnce = 0;
    /* Wave 35 exclusive soft deepen tallies. */
    g_u32SoftWaitPeak = 0;
    g_u32SoftDeadlkProbe = 0;
    g_u32SoftDeadlkHit = 0;
    g_u32SoftEofGrant = 0;
    g_u32SoftFiniteGrant = 0;
    g_u32SoftWakeZero = 0;
    g_u32SoftWakeSome = 0;
    g_u32SoftBlockRetry = 0;
    g_u32SoftLastType = 0;
    g_u32SoftLastDenyCode = 0;
    gj_spin_init(&g_lkLock);
    kprintf("file_lock: init slots=%u waiters=%u locks=0\n", GJ_FLOCK_MAX,
            GJ_FLOCK_MAX_WAITERS);
    /* Grep: file_lock: soft (baseline inventory after init) */
    soft_inventory_log();
}

/**
 * Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit).
 * greppable helper for Wave 13 path tallies.
 */
static void
soft_inc(u32 *pu32Ctr)
{
    if (pu32Ctr == NULL) {
        return;
    }
    if (*pu32Ctr < 0xffffffffu) {
        (*pu32Ctr)++;
    }
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
 * Note live waiter high-water. Caller holds g_lkLock.
 */
static void
soft_wait_peak_note(void)
{
    if (g_u32NWaiters > g_u32SoftWaitPeak) {
        g_u32SoftWaitPeak = g_u32NWaiters;
    }
}

/**
 * Bump soft deny total + optional typed bucket. Caller holds g_lkLock or
 * is on a terminal return path before unlock is fine either way (u32 only).
 */
static void
soft_deny_note(u32 *pu32Bucket)
{
    soft_inc(&g_u32SoftDeny);
    if (pu32Bucket != NULL) {
        soft_inc(pu32Bucket);
    }
    /* Wave 15: last deny code snapshot. */
    if (pu32Bucket == &g_u32SoftDenyEagain) {
        g_u32SoftLastDenyCode = 1u;
    } else if (pu32Bucket == &g_u32SoftDenyDeadlk) {
        g_u32SoftLastDenyCode = 2u;
    } else if (pu32Bucket == &g_u32SoftDenyNolck) {
        g_u32SoftLastDenyCode = 3u;
    }
}

/**
 * Greppable soft lock inventory (product / smoke; Wave 35 exclusive deepen).
 * Primary field-stable lines (Wave 8):
 *   file_lock: soft held=… free=… peak=… waiters=… gen=…
 *   file_lock: soft deny=… eagain=… deadlk=… nolck=… set_ok=… unlk_ok=…
 *   file_lock: soft get=… get_hit=… block=… wake=… wake_n=… rel_fd=… rel_pid=…
 * Wave 13 complementary sub-lines (agent greps):
 *   file_lock: soft inventory|held|deny|set|get|block|wake|wait|carve|
 *              release|types|path …
 * Wave 15 complementary:
 *   file_lock: soft total|rate|deadlk|grant|catalog|deepen
 * greppable: file_lock: soft
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
    u32 u32Rd = 0;
    u32 u32Wr = 0;
    u32 u32Eof = 0;
    u32 u32WaitFree;
    u32 u32Slot;
    u32 u32OccPct;
    u32 u32DenyShare;
    u32 u32OkBp;
    u32 u32WaitPeak;

    soft_inc(&g_u32SoftLogN);

    gj_spin_lock(&g_lkLock);
    u32Held = g_u32NLocks;
    u32Wait = g_u32NWaiters;
    u32Gen = g_u32WakeGen;
    /* Bound free so soft inventory never wraps if counters ever desync. */
    u32Free = (u32Held < (u32)GJ_FLOCK_MAX) ? ((u32)GJ_FLOCK_MAX - u32Held)
                                            : 0u;
    u32WaitFree = (u32Wait < (u32)GJ_FLOCK_MAX_WAITERS)
                      ? ((u32)GJ_FLOCK_MAX_WAITERS - u32Wait)
                      : 0u;
    u32Peak = g_u32SoftHeldPeak;
    u32WaitPeak = g_u32SoftWaitPeak;
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
    /* Live type / EOF scan (diagnostic; under lock). */
    for (u32Slot = 0; u32Slot < GJ_FLOCK_MAX; u32Slot++) {
        if (!g_aLk[u32Slot].u8Used) {
            continue;
        }
        if (g_aLk[u32Slot].i16Type == GJ_F_RDLCK) {
            u32Rd++;
        } else if (g_aLk[u32Slot].i16Type == GJ_F_WRLCK) {
            u32Wr++;
        }
        if (g_aLk[u32Slot].i64End < 0) {
            u32Eof++;
        }
    }
    gj_spin_unlock(&g_lkLock);

    /* Soft occupancy % (integer; capacity never 0). */
    u32OccPct = (u32Held * 100u) / (u32)GJ_FLOCK_MAX;
    /* Soft deny share of set enters (basis points; 0 if none entered). */
    if (g_u32SoftSetEnter != 0u) {
        u32DenyShare = (u32Deny * 10000u) / g_u32SoftSetEnter;
        u32OkBp = (u32SetOk * 10000u) / g_u32SoftSetEnter;
    } else {
        u32DenyShare = 0;
        u32OkBp = 0;
    }

    /*
     * Primary field-stable lines (Wave 8 shape). Keep field names so
     * existing greps on held=/deny=/get= continue to match.
     */
    /* Grep: file_lock: soft held= */
    kprintf("file_lock: soft held=%u free=%u peak=%u waiters=%u gen=%u\n",
            u32Held, u32Free, u32Peak, u32Wait, u32Gen);
    /* Grep: file_lock: soft deny= */
    kprintf("file_lock: soft deny=%u eagain=%u deadlk=%u nolck=%u "
            "set_ok=%u unlk_ok=%u\n",
            u32Deny, u32Eagain, u32Deadlk, u32Nolck, u32SetOk, u32UnlkOk);
    /* Grep: file_lock: soft get= */
    kprintf("file_lock: soft get=%u get_hit=%u block=%u wake=%u wake_n=%u "
            "rel_fd=%u rel_pid=%u\n",
            u32Get, u32GetHit, u32Block, u32Wake, u32WakeN, u32RelFd,
            u32RelPid);

    /*
     * Wave 13 exclusive deepen (complementary; never reshapes primary lines).
     * Wave 15 bumps wave stamp + adds total/rate/deadlk/grant/catalog/deepen.
     */
    /* Grep: file_lock: soft inventory */
    kprintf("file_lock: soft inventory wave=71 slots=%u waiters_max=%u "
            "wake_budget=%u deadlk_depth=%u held=%u free=%u peak=%u "
            "waiters=%u gen=%u occ_pct=%u deny_bp=%u log_n=%u "
            "count_sample=%u gen_sample=%u "
            "(advisory soft inventory; not bar3)\n",
            GJ_FLOCK_MAX, GJ_FLOCK_MAX_WAITERS, GJ_FLOCK_SOFT_WAKE_MAX,
            GJ_FLOCK_DEADLOCK_DEPTH, u32Held, u32Free, u32Peak, u32Wait,
            u32Gen, u32OccPct, u32DenyShare, g_u32SoftLogN,
            g_u32SoftCountSample, g_u32SoftGenSample);

    /* Grep: file_lock: soft held  (word form; primary held= stays above) */
    kprintf("file_lock: soft held live=%u free=%u peak=%u slots=%u "
            "occ_pct=%u rd=%u wr=%u eof=%u\n",
            u32Held, u32Free, u32Peak, GJ_FLOCK_MAX, u32OccPct, u32Rd, u32Wr,
            u32Eof);

    /* Grep: file_lock: soft deny  (word form) */
    kprintf("file_lock: soft deny total=%u eagain=%u deadlk=%u nolck=%u "
            "share_bp=%u set_enter=%u\n",
            u32Deny, u32Eagain, u32Deadlk, u32Nolck, u32DenyShare,
            g_u32SoftSetEnter);

    /* Grep: file_lock: soft set */
    kprintf("file_lock: soft set enter=%u ok=%u rd=%u wr=%u unlk_enter=%u "
            "unlk_ok=%u val_fail=%u type_inval=%u\n",
            g_u32SoftSetEnter, u32SetOk, g_u32SoftSetRd, g_u32SoftSetWr,
            g_u32SoftUnlkEnter, u32UnlkOk, g_u32SoftValFail,
            g_u32SoftTypeInval);

    /* Grep: file_lock: soft get  (word form) */
    kprintf("file_lock: soft get probes=%u hit=%u miss=%u unlk_probe=%u "
            "val_fail=%u\n",
            u32Get, u32GetHit, g_u32SoftGetMiss, g_u32SoftGetUnlkProbe,
            g_u32SoftGetValFail);

    /* Grep: file_lock: soft block */
    kprintf("file_lock: soft block enter=%u thr=%u yield=%u lost_wake_self=%u "
            "deadlk=%u eagain=%u retry=%u\n",
            u32Block, g_u32SoftBlockThr, g_u32SoftBlockYield,
            g_u32SoftLostWakeSelf, u32Deadlk, u32Eagain, g_u32SoftBlockRetry);

    /* Grep: file_lock: soft wake */
    kprintf("file_lock: soft wake calls=%u woken=%u gen=%u gen_bump=%u "
            "budget=%u tag=%u zero=%u some=%u\n",
            u32Wake, u32WakeN, u32Gen, g_u32SoftWakeGenBump,
            GJ_FLOCK_SOFT_WAKE_MAX, GJ_FLOCK_TAG_WAITER, g_u32SoftWakeZero,
            g_u32SoftWakeSome);

    /* Grep: file_lock: soft wait */
    kprintf("file_lock: soft wait live=%u free=%u max=%u reg=%u full=%u "
            "unreg=%u samples=%u peak=%u\n",
            u32Wait, u32WaitFree, GJ_FLOCK_MAX_WAITERS, g_u32SoftWaitReg,
            g_u32SoftWaitFull, g_u32SoftWaitUnreg, g_u32SoftWaitSample,
            u32WaitPeak);

    /* Grep: file_lock: soft carve */
    kprintf("file_lock: soft carve ok=%u nolck=%u split_hole=%u coalesce=%u "
            "conflict=%u\n",
            g_u32SoftCarveOk, g_u32SoftCarveNolck, g_u32SoftSplitHole,
            g_u32SoftCoalesce, g_u32SoftConflict);

    /* Grep: file_lock: soft release */
    kprintf("file_lock: soft release fd_call=%u fd_slots=%u fd_zero=%u "
            "pid_call=%u pid_slots=%u pid_zero=%u wait_drop=%u\n",
            g_u32SoftRelFdCall, u32RelFd, g_u32SoftRelFdZero,
            g_u32SoftRelPidCall, u32RelPid, g_u32SoftRelPidZero,
            g_u32SoftRelWaitDrop);

    /* Grep: file_lock: soft types */
    kprintf("file_lock: soft types rd=%u wr=%u eof=%u set_rd=%u set_wr=%u "
            "held=%u\n",
            u32Rd, u32Wr, u32Eof, g_u32SoftSetRd, g_u32SoftSetWr, u32Held);

    /*
     * Wave 35 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: file_lock: soft total */
    kprintf("file_lock: soft total set_ok=%u unlk_ok=%u deny=%u get=%u "
            "block=%u wake=%u logs=%u wave=71\n",
            u32SetOk, u32UnlkOk, u32Deny, u32Get, u32Block, u32Wake,
            g_u32SoftLogN);

    /* Grep: file_lock: soft rate */
    kprintf("file_lock: soft rate ok_bp=%u deny_bp=%u set_enter=%u "
            "occ_pct=%u wait_peak=%u held_peak=%u\n",
            u32OkBp, u32DenyShare, g_u32SoftSetEnter, u32OccPct, u32WaitPeak,
            u32Peak);

    /* Grep: file_lock: soft deadlk */
    kprintf("file_lock: soft deadlk probe=%u hit=%u deny=%u depth=%u "
            "last_deny=%u wave=71\n",
            g_u32SoftDeadlkProbe, g_u32SoftDeadlkHit, u32Deadlk,
            GJ_FLOCK_DEADLOCK_DEPTH, g_u32SoftLastDenyCode);

    /* Grep: file_lock: soft grant */
    kprintf("file_lock: soft grant eof=%u finite=%u rd=%u wr=%u "
            "last_type=%u\n",
            g_u32SoftEofGrant, g_u32SoftFiniteGrant, g_u32SoftSetRd,
            g_u32SoftSetWr, g_u32SoftLastType);

    /* Grep: file_lock: soft catalog */
    kprintf("file_lock: soft catalog slots=%u waiters_max=%u wake_budget=%u "
            "deadlk_depth=%u tag=%u eof_sent=-1 wave=71\n",
            GJ_FLOCK_MAX, GJ_FLOCK_MAX_WAITERS, GJ_FLOCK_SOFT_WAKE_MAX,
            GJ_FLOCK_DEADLOCK_DEPTH, GJ_FLOCK_TAG_WAITER);

    /* Grep: file_lock: soft capacity — Wave 19 design-constant lamps. */
    kprintf("file_lock: soft capacity slots=%u waiters_max=%u "
            "wake_budget=%u deadlk_depth=%u multi_waiter=1 wave=71\n",
            GJ_FLOCK_MAX, GJ_FLOCK_MAX_WAITERS, GJ_FLOCK_SOFT_WAKE_MAX,
            GJ_FLOCK_DEADLOCK_DEPTH);

    /* Grep: file_lock: soft headroom — Wave 19 live slack lamps. */
    kprintf("file_lock: soft headroom free=%u held=%u peak=%u "
            "waiters=%u wait_free=%u wait_peak=%u occ_pct=%u wave=71\n",
            u32Free, u32Held, u32Peak, u32Wait, u32WaitFree, u32WaitPeak,
            u32OccPct);

    /* Grep: file_lock: soft surface — Wave 19 surface bit lamps. */
    kprintf("file_lock: soft surface held=%u waiters=%u deny=%u set_ok=%u "
            "block=%u wake=%u surf=0x%x wave=71\n",
            u32Held != 0u ? 1u : 0u, u32Wait != 0u ? 1u : 0u,
            u32Deny != 0u ? 1u : 0u, u32SetOk != 0u ? 1u : 0u,
            u32Block != 0u ? 1u : 0u, u32Wake != 0u ? 1u : 0u,
            ((u32Held != 0u) ? 1u : 0u) | ((u32Wait != 0u) ? 2u : 0u) |
                ((u32Deny != 0u) ? 4u : 0u) | ((u32SetOk != 0u) ? 8u : 0u) |
                ((u32Block != 0u) ? 16u : 0u) | ((u32Wake != 0u) ? 32u : 0u));

    /* Grep: file_lock: soft terminal — Wave 19 outcome rollup. */
    kprintf("file_lock: soft terminal set_ok=%u unlk_ok=%u deny=%u get=%u "
            "block=%u wake=%u soft PASS wave=71\n",
            u32SetOk, u32UnlkOk, u32Deny, u32Get, u32Block, u32Wake);

    /* Grep: file_lock: soft return — Wave 19 API return surfaces */
    kprintf("file_lock: soft return set_ok=%u unlk_ok=%u get=%u get_hit=%u "
            "eagain=%u edeadlk=%u enolck=%u block=%u wake=%u "
            "release_fd=%u release_pid=%u product_flock=OPEN wave=71\n",
            u32SetOk, u32UnlkOk, u32Get, u32GetHit,
            u32Eagain, u32Deadlk, u32Nolck, u32Block, u32Wake,
            u32RelFd, u32RelPid);

    /* Grep: file_lock: soft retmap — Wave 19 return-surface map */
    kprintf("file_lock: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=71\n");

    /* Grep: file_lock: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: file_lock: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("file_lock: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=71 "
            "(retclass taxonomy; Soft≠product; not bar3)\n");
    /* Grep: file_lock: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("file_lock: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=71 "
            "(retlane catalog; Soft≠product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: file_lock: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("file_lock: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=71 "
            "(retbound honesty; Soft≠product; not bar3)\n");
    /* Grep: file_lock: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("file_lock: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=71 "
            "(retseal stamp; Soft≠product)\n");
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: file_lock: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("file_lock: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(retpulse honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("file_lock: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retmark stamp; Soft≠product)\n");
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: file_lock: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("file_lock: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(retphase honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("file_lock: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: file_lock: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("file_lock: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(rettoken honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("file_lock: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retcrest stamp; Soft≠product)\n");
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: file_lock: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("file_lock: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(retvault honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("file_lock: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retbanner stamp; Soft≠product)\n");
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: file_lock: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("file_lock: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(retledger honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("file_lock: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retbeacon stamp; Soft≠product)\n");
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: file_lock: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("file_lock: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=71 "
                    "(retcipher honesty; Soft≠product; not bar3)\n");
            /* Grep: file_lock: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("file_lock: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=71 "
                    "(retflame stamp; Soft≠product)\n");
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: file_lock: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("file_lock: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=71 "
                            "(retprism honesty; Soft≠product; not bar3)\n");
                    /* Grep: file_lock: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("file_lock: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=71 "
                            "(retforge stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: file_lock: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("file_lock: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=71 "
                                    "(retshard honesty; Soft≠product; not bar3)\n");
                            /* Grep: file_lock: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("file_lock: soft retcrown exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=71 "
                                    "(retcrown stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: file_lock: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("file_lock: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=71 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: file_lock: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("file_lock: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=71 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: file_lock: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("file_lock: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=71 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: file_lock: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("file_lock: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=71 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: file_lock: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("file_lock: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=71 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: file_lock: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("file_lock: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=71 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: file_lock: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("file_lock: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=71 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("file_lock: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("file_lock: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("file_lock: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("file_lock: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("file_lock: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("file_lock: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retfortress — Wave 35 return-fortress honesty */
kprintf("file_lock: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("file_lock: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft rethold — Wave 36 return-hold honesty */
kprintf("file_lock: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retspire — Wave 36 exclusive spire stamp */
kprintf("file_lock: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retwall — Wave 37 return-wall honesty */
kprintf("file_lock: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retgate — Wave 37 exclusive gate stamp */
kprintf("file_lock: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retmoat — Wave 38 return-moat honesty */
kprintf("file_lock: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retower — Wave 38 exclusive tower stamp */
kprintf("file_lock: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("file_lock: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("file_lock: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("file_lock: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("file_lock: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retravelin — Wave 41 return-travelin honesty */
kprintf("file_lock: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("file_lock: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("file_lock: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("file_lock: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("file_lock: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("file_lock: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("file_lock: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("file_lock: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("file_lock: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("file_lock: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retbailey — Wave 46 return-bailey honesty */
kprintf("file_lock: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("file_lock: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("file_lock: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("file_lock: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("file_lock: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("file_lock: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("file_lock: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("file_lock: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retsally — Wave 50 return-sally honesty */
kprintf("file_lock: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("file_lock: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retfosse — Wave 51 return-fosse honesty */
kprintf("file_lock: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("file_lock: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("file_lock: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("file_lock: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retravelin — Wave 53 return-travelin honesty */
kprintf("file_lock: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("file_lock: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("file_lock: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retredan — Wave 54 exclusive redan stamp */
kprintf("file_lock: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retflank — Wave 55 return-flank honesty */
kprintf("file_lock: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retface — Wave 55 exclusive face stamp */
kprintf("file_lock: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retgorge — Wave 56 return-gorge honesty */
kprintf("file_lock: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("file_lock: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retraverse — Wave 57 return-traverse honesty */
kprintf("file_lock: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("file_lock: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retorillon — Wave 58 return-orillon honesty */
kprintf("file_lock: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("file_lock: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("file_lock: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("file_lock: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retplace — Wave 60 return-place honesty */
kprintf("file_lock: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("file_lock: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("file_lock: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("file_lock: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("file_lock: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("file_lock: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("file_lock: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("file_lock: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: file_lock: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("file_lock: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: file_lock: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("file_lock: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: file_lock: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("file_lock: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: file_lock: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("file_lock: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: file_lock: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("file_lock: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=71 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: file_lock: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("file_lock: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=71 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("file_lock: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("file_lock: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("file_lock: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("file_lock: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("file_lock: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=71 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("file_lock: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=71 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("file_lock: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=71 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("file_lock: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=71 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: file_lock: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("file_lock: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=71 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: file_lock: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("file_lock: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=71 (retreentrantangle stamp; Soft≠product)\n");

                            kprintf("file_lock: soft deepen wave=71 areas=total,rate,deadlk,grant,,retclass,retlane"
            "catalog,capacity,headroom,surface,terminal,return,wait_peak,"
            "wake_zero logs=%u "
            "(Wave 35 exclusive; advisory soft inventory; not bar3)\n",
            g_u32SoftLogN);

    /* Grep: file_lock: soft path */
    kprintf("file_lock: soft path claim=advisory_flock set=carve+insert "
            "get=conflict_probe block=thread_block+schedule "
            "wake=multi_budget coalesce=adjacent_same_type "
            "split=FLOCK_CONFLICT_SPLIT multi=FLOCK_SOFT_MULTI_WAITER "
            "deadlk=FLOCK_SOFT_DEADLOCK soft_honesty=soft wave=71 "
            "(advisory soft inventory; not bar3)\n");
}

/**
 * After first product activity, print soft inventory once (mirrors
 * io_uring / session_door soft-stats-once). Safe from set/get/release
 * return paths. greppable: file_lock: soft
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftSetOk == 0 && g_u32SoftUnlkOk == 0 && g_u32SoftGet == 0 &&
        g_u32SoftRelFd == 0 && g_u32SoftRelPid == 0 &&
        g_u32SoftDeny == 0 && g_u32SoftBlock == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log();
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
    soft_inc(&g_u32SoftWakeGenBump);
}

static u32
soft_wake_waiters(void)
{
    u32 u32N;

    /* All soft waiters share g_u32WaitObj; they re-check conflict. */
    u32N = thread_wake(&g_u32WaitObj, GJ_FLOCK_TAG_WAITER, GJ_FLOCK_SOFT_WAKE_MAX);
    soft_inc(&g_u32SoftWakeCalls);
    g_u32SoftWakeN += u32N;
    if (u32N == 0u) {
        soft_inc(&g_u32SoftWakeZero); /* Wave 15 */
    } else {
        soft_inc(&g_u32SoftWakeSome);
    }
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
        soft_inc(&g_u32SoftConflict); /* Wave 13: conflict sample */
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
                soft_inc(&g_u32SoftCarveNolck);
                return -LINUX_ENOLCK;
            }
            /* Left remnant in place. */
            pS->i64End = i64U0;
            pR = slot_alloc();
            if (pR == NULL) {
                /* Restore original end on failure. */
                pS->i64End = i64L1;
                soft_inc(&g_u32SoftCarveNolck);
                return -LINUX_ENOLCK;
            }
            slot_fill(pR, i64Fd, i16Type, i64U1, i64L1, u32Pid);
            soft_inc(&g_u32SoftSplitHole); /* Wave 13: FLOCK_CONFLICT_SPLIT */
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
    soft_inc(&g_u32SoftCarveOk);
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
            soft_inc(&g_u32SoftCoalesce); /* Wave 13: FLOCK_COALESCE */
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
            soft_wait_peak_note(); /* Wave 15: waiter high-water */
            soft_inc(&g_u32SoftWaitReg);
            return (int)iW;
        }
    }
    soft_inc(&g_u32SoftWaitFull);
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
    soft_inc(&g_u32SoftWaitUnreg);
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

    soft_inc(&g_u32SoftDeadlkProbe); /* Wave 15: probe sample */
    if (u32BlockerPid == 0 || u32SelfPid == 0) {
        return 0;
    }
    if (u32BlockerPid == u32SelfPid) {
        soft_inc(&g_u32SoftDeadlkHit);
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
            soft_inc(&g_u32SoftDeadlkHit);
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

    soft_inc(&g_u32SoftSetEnter); /* Wave 13: set path enter */
    if (pFl != NULL) {
        /* Wave 15: last type snapshot (0=unlk, 1=rd, 2=wr, 3=other). */
        if (pFl->i16Type == GJ_F_UNLCK) {
            g_u32SoftLastType = 0u;
        } else if (pFl->i16Type == GJ_F_RDLCK) {
            g_u32SoftLastType = 1u;
        } else if (pFl->i16Type == GJ_F_WRLCK) {
            g_u32SoftLastType = 2u;
        } else {
            g_u32SoftLastType = 3u;
        }
    }

    i64St = flock_validate(i64Fd, pFl, &i64Start, &i64End, &u32Pid);
    if (i64St != 0) {
        soft_inc(&g_u32SoftValFail);
        return i64St;
    }

    if (pFl->i16Type == GJ_F_UNLCK) {
        soft_inc(&g_u32SoftUnlkEnter);
        gj_spin_lock(&g_lkLock);
        i64St = carve_pid_range(i64Fd, u32Pid, i64Start, i64End);
        if (i64St == 0) {
            wake_gen_bump();
            soft_inc(&g_u32SoftUnlkOk);
        } else if (i64St == -LINUX_ENOLCK) {
            soft_deny_note(&g_u32SoftDenyNolck);
        }
        gj_spin_unlock(&g_lkLock);
        if (i64St == 0) {
            (void)soft_wake_waiters();
        }
        soft_inventory_maybe_once();
        return i64St;
    }
    if (pFl->i16Type != GJ_F_RDLCK && pFl->i16Type != GJ_F_WRLCK) {
        soft_inc(&g_u32SoftTypeInval);
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
                soft_inventory_maybe_once();
                return -LINUX_EAGAIN;
            }
            /* Soft deadlock before sleeping. greppable: FLOCK_SOFT_DEADLOCK */
            if (soft_deadlock(u32Pid, u32Blocker)) {
                soft_deny_note(&g_u32SoftDenyDeadlk);
                if (fRegistered) {
                    waiter_unregister(nWait);
                }
                gj_spin_unlock(&g_lkLock);
                soft_inventory_maybe_once();
                return -(i64)GJ_FLOCK_EDEADLK;
            }
            soft_inc(&g_u32SoftBlock);
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
                soft_inc(&g_u32SoftBlockThr);
                thread_block(&g_u32WaitObj, GJ_FLOCK_TAG_WAITER);
                gj_spin_lock(&g_lkLock);
                if (!conflict(i64Fd, pFl->i16Type, i64Start, i64End, u32Pid,
                              NULL)) {
                    gj_spin_unlock(&g_lkLock);
                    soft_inc(&g_u32SoftLostWakeSelf);
                    (void)soft_wake_waiters();
                } else {
                    gj_spin_unlock(&g_lkLock);
                }
                schedule();
            } else {
                soft_inc(&g_u32SoftBlockYield);
                thread_yield();
            }
            soft_inc(&g_u32SoftBlockRetry); /* Wave 15: re-enter loop */
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
            soft_inventory_maybe_once();
            return i64St;
        }
        pNew = slot_alloc();
        if (pNew == NULL) {
            soft_deny_note(&g_u32SoftDenyNolck);
            if (fRegistered) {
                waiter_unregister(nWait);
            }
            gj_spin_unlock(&g_lkLock);
            soft_inventory_maybe_once();
            return -LINUX_ENOLCK;
        }
        slot_fill(pNew, i64Fd, pFl->i16Type, i64Start, i64End, u32Pid);
        coalesce_pid(i64Fd, u32Pid);
        soft_inc(&g_u32SoftSetOk);
        if (pFl->i16Type == GJ_F_RDLCK) {
            soft_inc(&g_u32SoftSetRd);
        } else {
            soft_inc(&g_u32SoftSetWr);
        }
        /* Wave 15: eof vs finite grant shape. */
        if (i64End < 0) {
            soft_inc(&g_u32SoftEofGrant);
        } else {
            soft_inc(&g_u32SoftFiniteGrant);
        }
        g_u32SoftLastDenyCode = 0u;
        if (fRegistered) {
            waiter_unregister(nWait);
        }
        /* SET may upgrade read→write and free readers' soft wait paths. */
        wake_gen_bump();
        gj_spin_unlock(&g_lkLock);
        (void)soft_wake_waiters();
        soft_inventory_maybe_once();
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
        soft_inc(&g_u32SoftGetValFail);
        return i64St;
    }

    /* UNLCK probe → treat as write so any lock is reported (POSIX F_GETLK). */
    if (pFlInOut->i16Type == GJ_F_UNLCK) {
        soft_inc(&g_u32SoftGetUnlkProbe);
        i16Probe = GJ_F_WRLCK;
    } else {
        i16Probe = pFlInOut->i16Type;
    }
    if (i16Probe != GJ_F_RDLCK && i16Probe != GJ_F_WRLCK) {
        soft_inc(&g_u32SoftTypeInval);
        return -LINUX_EINVAL;
    }

    gj_spin_lock(&g_lkLock);
    soft_inc(&g_u32SoftGet);
    if (conflict(i64Fd, i16Probe, i64Start, i64End, u32Pid, &pHit) &&
        pHit != NULL) {
        pFlInOut->i16Type = pHit->i16Type;
        pFlInOut->i16Whence = GJ_SEEK_SET;
        pFlInOut->i64Start = pHit->i64Start;
        pFlInOut->i64Len =
            pHit->i64End < 0 ? 0 : (pHit->i64End - pHit->i64Start);
        pFlInOut->u32Pid = pHit->u32Pid;
        soft_inc(&g_u32SoftGetHit);
        gj_spin_unlock(&g_lkLock);
        soft_inventory_maybe_once();
        return 0;
    }
    gj_spin_unlock(&g_lkLock);
    soft_inc(&g_u32SoftGetMiss);
    pFlInOut->i16Type = GJ_F_UNLCK;
    pFlInOut->i16Whence = GJ_SEEK_SET;
    soft_inventory_maybe_once();
    return 0;
}

u32
file_lock_count(void)
{
    u32 u32N;

    soft_inc(&g_u32SoftCountSample);
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

    soft_inc(&g_u32SoftWaitSample);
    gj_spin_lock(&g_lkLock);
    u32N = g_u32NWaiters;
    gj_spin_unlock(&g_lkLock);
    return u32N;
}

u32
file_lock_wake_gen(void)
{
    u32 u32G;

    soft_inc(&g_u32SoftGenSample);
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

    soft_inc(&g_u32SoftRelFdCall);
    if (i64Fd < 0) {
        soft_inc(&g_u32SoftRelFdZero);
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
    } else {
        soft_inc(&g_u32SoftRelFdZero);
    }
    gj_spin_unlock(&g_lkLock);
    if (u32Cleared > 0) {
        (void)soft_wake_waiters();
    }
    soft_inventory_maybe_once();
    return u32Cleared;
}

u32
file_lock_release_pid(u32 u32Pid)
{
    u32 iSlot;
    u32 u32Cleared = 0;
    u32 iW;
    u32 u32WaitDrop = 0;

    soft_inc(&g_u32SoftRelPidCall);
    if (u32Pid == 0) {
        soft_inc(&g_u32SoftRelPidZero);
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
            u32WaitDrop++;
        }
    }
    g_u32SoftRelWaitDrop += u32WaitDrop;
    if (u32Cleared > 0) {
        wake_gen_bump();
        g_u32SoftRelPid += u32Cleared;
    } else {
        soft_inc(&g_u32SoftRelPidZero);
    }
    gj_spin_unlock(&g_lkLock);
    if (u32Cleared > 0) {
        (void)soft_wake_waiters();
    }
    soft_inventory_maybe_once();
    return u32Cleared;
}
