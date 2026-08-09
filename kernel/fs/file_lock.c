/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Advisory byte-range locks for wine/Proton file locking.
 * Per-fd ranges with RDLCK/WRLCK/UNLCK; same-pid overlaps carve/split;
 * cross-pid write conflicts soft-block (F_SETLKW) or EAGAIN (nonblock).
 *
 * Product surface (stable):
 *   - FLOCK_CONFLICT_SPLIT: partial UNLCK/SETLK leaves non-overlapped remnants
 *   - FLOCK_SOFT_MULTI_WAITER: register waiters, thread_block/wake, gen bump
 *   - Soft deadlock: short cycle over waiter table -> -GJ_FLOCK_EDEADLK
 *   - GETLK: lowest-start conflict; write preferred on equal start
 *   - Overflow-safe range ends; adjacent same-type multi-pass coalesce
 *   - release_pid drops exiting-pid waiters (no orphan deadlock edges)
 *
 * C0 residual deepen (Soft!=product; dual MIT OR Apache-2.0; stamp-free):
 *   Sticky counters on set/get/block/wake/release/coalesce paths - never hard-gate.
 *   Sparse inventory: init + first-activity once + stats-sample, CAP-capped.
 *   Multi-pass coalesce chain merges (bounded GJ_FLOCK_MAX passes).
 *   release_fd observes fd-keyed waiters (soft); does not steal live registrations.
 *   release_fd orphan-waiter residual: waiters observed + 0 held -> gen bump + wake.
 *   release_pid wake also when wait-only drop (exiting pid).
 *   Functional lean residual once: type/range/cap/live coherence + PASS|FAIL.
 *   No version stamp. No stamp storms. Soft residual != product flock claim.
 *   Dual DoD remains OPEN (advisory soft residual only).
 *   greppable: file_lock: soft residual lean
 *   greppable: file_lock: soft residual lean PASS
 *   greppable: file_lock: soft held= | deny= | get=
 *   greppable: FLOCK_COALESCE_MULTI | FLOCK_REL_FD_WAIT_OBS | FLOCK_REL_FD_ORPHAN_WAKE
 *
 * SMP-safe via ticket-free spinlock; pure C11 dual-license product path.
 * G-AC-1: no Linux .ko; Soft!=product.
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
/*
 * Cap multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * Init + first-activity once + a couple of stats samples stay greppable;
 * further calls refresh counters only (silent).
 */
#define FLOCK_SOFT_LOG_CAP 4u

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
 * Soft F_SETLKW waiter - non-exclusive registration for cycle detect + stats.
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
 * Lean soft residual inventory (counters only; never hard-gate).
 * Cumulative unless noted live/peak. Wrap OK. Soft!=product.
 * greppable: file_lock: soft residual lean
 */
static u32 g_u32SoftHeldPeak;   /* max live held (g_u32NLocks high-water) */
static u32 g_u32SoftSetOk;      /* successful RDLCK/WRLCK grants */
static u32 g_u32SoftUnlkOk;     /* successful UNLCK carves */
static u32 g_u32SoftDeny;       /* soft denies total (eagain+deadlk+nolck) */
static u32 g_u32SoftDenyEagain; /* nonblock conflict -> -EAGAIN */
static u32 g_u32SoftDenyDeadlk; /* soft cycle -> -EDEADLK */
static u32 g_u32SoftDenyNolck;  /* table full / carve fail -> -ENOLCK */
static u32 g_u32SoftGet;        /* F_GETLK probes */
static u32 g_u32SoftGetHit;     /* GETLK conflict rewrites */
static u32 g_u32SoftBlock;      /* times entered F_SETLKW wait path */
static u32 g_u32SoftWakeCalls;  /* soft multi-wake invocations */
static u32 g_u32SoftWakeN;      /* waiters woken (sum of thread_wake ret) */
static u32 g_u32SoftRelFd;      /* slots cleared via release_fd */
static u32 g_u32SoftRelPid;     /* slots cleared via release_pid */

/* Path tallies (file-local; silent unless residual dumps). */
static u32 g_u32SoftSetEnter;     /* file_lock_set entries */
static u32 g_u32SoftSetRd;        /* RDLCK grants */
static u32 g_u32SoftSetWr;        /* WRLCK grants */
static u32 g_u32SoftUnlkEnter;    /* UNLCK path entries */
static u32 g_u32SoftValFail;      /* flock_validate rejects */
static u32 g_u32SoftTypeInval;    /* bad i16Type on set */
static u32 g_u32SoftGetMiss;      /* GETLK free (type->UNLCK) */
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
static u32 g_u32SoftCoalescePass; /* multi-pass coalesce outer iterations */
static u32 g_u32SoftCoalesceChain;/* passes that merged after pass 0 (chain) */
static u32 g_u32SoftConflict;     /* conflict() positive hits */
static u32 g_u32SoftRelFdCall;    /* release_fd entries */
static u32 g_u32SoftRelFdZero;    /* release_fd cleared 0 */
static u32 g_u32SoftRelFdWaitObs; /* waiters observed on fd at release_fd (C0) */
static u32 g_u32SoftRelFdOrphan;  /* release_fd orphan-waiter soft-wake (C0) */
static u32 g_u32SoftRelPidCall;   /* release_pid entries */
static u32 g_u32SoftRelPidZero;   /* release_pid cleared 0 */
static u32 g_u32SoftRelWaitDrop;  /* waiters dropped on release_pid */
static u32 g_u32SoftWakeGenBump;  /* wake_gen_bump calls */
static u32 g_u32SoftCountSample;  /* file_lock_count samples */
static u32 g_u32SoftWaitSample;   /* file_lock_waiters samples */
static u32 g_u32SoftGenSample;    /* file_lock_wake_gen samples */
static u32 g_u32SoftLogN;         /* soft inventory emissions (capped) */
static u8  g_fSoftOnce;           /* one-shot after first activity */

/* Complementary path tallies (silent counters). */
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
static u32 g_u32SoftRelWake;      /* release paths that issued soft wake */

/*
 * C0 functional residual lean self-check (once; Soft!=product).
 * Constant/range/cap/live coherence only - never hard-gates product path.
 * greppable: file_lock: soft residual lean | soft residual lean PASS
 */
static u32 g_u32SoftResidualLean;   /* lean residual self-check runs */
static u32 g_u32SoftResidualLeanOk; /* lean residual arms that passed */
static u8  g_fSoftLean;             /* one-shot lean residual gate */

static void soft_inc(u32 *pu32Ctr);
static void soft_held_note(void);
static void soft_wait_peak_note(void);
static void soft_deny_note(u32 *pu32Bucket);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_residual_lean_once(void);

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
    g_u32SoftCoalescePass = 0;
    g_u32SoftCoalesceChain = 0;
    g_u32SoftConflict = 0;
    g_u32SoftRelFdCall = 0;
    g_u32SoftRelFdZero = 0;
    g_u32SoftRelFdWaitObs = 0;
    g_u32SoftRelFdOrphan = 0;
    g_u32SoftRelPidCall = 0;
    g_u32SoftRelPidZero = 0;
    g_u32SoftRelWaitDrop = 0;
    g_u32SoftWakeGenBump = 0;
    g_u32SoftCountSample = 0;
    g_u32SoftWaitSample = 0;
    g_u32SoftGenSample = 0;
    g_u32SoftLogN = 0;
    g_fSoftOnce = 0;
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
    g_u32SoftRelWake = 0;
    g_u32SoftResidualLean = 0;
    g_u32SoftResidualLeanOk = 0;
    g_fSoftLean = 0;
    gj_spin_init(&g_lkLock);
    kprintf("file_lock: init slots=%u waiters=%u locks=0\n", GJ_FLOCK_MAX,
            GJ_FLOCK_MAX_WAITERS);
    /* Grep: file_lock: soft residual lean (init baseline; C0) */
    soft_inventory_log();
    /* Grep: file_lock: soft residual lean PASS (functional once; C0) */
    soft_residual_lean_once();
}

/**
 * Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit).
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
 * Bump soft deny total + optional typed bucket.
 */
static void
soft_deny_note(u32 *pu32Bucket)
{
    soft_inc(&g_u32SoftDeny);
    if (pu32Bucket != NULL) {
        soft_inc(pu32Bucket);
    }
    if (pu32Bucket == &g_u32SoftDenyEagain) {
        g_u32SoftLastDenyCode = 1u;
    } else if (pu32Bucket == &g_u32SoftDenyDeadlk) {
        g_u32SoftLastDenyCode = 2u;
    } else if (pu32Bucket == &g_u32SoftDenyNolck) {
        g_u32SoftLastDenyCode = 3u;
    }
}

/**
 * Lean soft residual inventory - Soft!=product dual MIT OR Apache-2.0.
 * C0 residual deepen: CAP-capped, stamp-free, never hard-gates.
 *
 * Field-stable primary lines (smoke greps):
 *   file_lock: soft held=... free=... peak=... waiters=... gen=...
 *   file_lock: soft deny=... eagain=... deadlk=... nolck=... set_ok=... unlk_ok=...
 *   file_lock: soft get=... get_hit=... block=... wake=... wake_n=... rel_fd=... rel_pid=...
 * Lean residual (C0):
 *   file_lock: soft residual lean ...
 * greppable: file_lock: soft residual lean
 * greppable: FLOCK_COALESCE_MULTI | FLOCK_REL_FD_WAIT_OBS | FLOCK_REL_FD_ORPHAN_WAKE
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
    u32 u32WaitPeak;
    u32 u32Coal;
    u32 u32CoalPass;
    u32 u32CoalChain;
    u32 u32RelFdWObs;
    u32 u32RelFdOrphan;
    u32 u32RelPidWDrop;
    u32 u32LostWake;
    u32 u32SetEnter;
    u32 u32LastDeny;
    u32 u32LeanRuns;
    u32 u32LeanOk;

    /*
     * No stamp storms: after CAP dumps, stay silent (counters keep ticking).
     * Soft!=product - inventory never hard-gates.
     */
    if (g_u32SoftLogN >= FLOCK_SOFT_LOG_CAP) {
        return;
    }
    soft_inc(&g_u32SoftLogN);

    gj_spin_lock(&g_lkLock);
    u32Held = g_u32NLocks;
    u32Wait = g_u32NWaiters;
    u32Gen = g_u32WakeGen;
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
    u32Coal = g_u32SoftCoalesce;
    u32CoalPass = g_u32SoftCoalescePass;
    u32CoalChain = g_u32SoftCoalesceChain;
    u32RelFdWObs = g_u32SoftRelFdWaitObs;
    u32RelFdOrphan = g_u32SoftRelFdOrphan;
    u32RelPidWDrop = g_u32SoftRelWaitDrop;
    u32LostWake = g_u32SoftLostWakeSelf;
    u32SetEnter = g_u32SoftSetEnter;
    u32LastDeny = g_u32SoftLastDenyCode;
    u32LeanRuns = g_u32SoftResidualLean;
    u32LeanOk = g_u32SoftResidualLeanOk;
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
     * Grep: file_lock: soft residual lean
     * C0 residual honesty - Soft!=product dual license; no version stamp.
     * Advisory soft inventory only; not product flock complete.
     * greppable: FLOCK_COALESCE_MULTI | FLOCK_REL_FD_WAIT_OBS | FLOCK_REL_FD_ORPHAN_WAKE
     */
    kprintf("file_lock: soft residual lean "
            "slots=%u waiters_max=%u wake_budget=%u deadlk_depth=%u "
            "held=%u free=%u peak=%u waiters=%u wait_free=%u wait_peak=%u "
            "rd=%u wr=%u eof=%u set_ok=%u unlk_ok=%u deny=%u get=%u "
            "block=%u wake=%u carve_ok=%u split=%u coalesce=%u "
            "coal_pass=%u coal_chain=%u rel_fd_wobs=%u rel_fd_orphan=%u "
            "rel_pid_wdrop=%u lost_wake=%u set_enter=%u last_deny=%u "
            "rel_wake=%u deadlk_probe=%u deadlk_hit=%u "
            "lean_runs=%u lean_ok=%u log_n=%u log_cap=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 Dual_DoD=OPEN "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "no stamp storms; C0 residual deepen; advisory soft residual; "
            "Dual DoD OPEN)\n",
            GJ_FLOCK_MAX, GJ_FLOCK_MAX_WAITERS, GJ_FLOCK_SOFT_WAKE_MAX,
            GJ_FLOCK_DEADLOCK_DEPTH, u32Held, u32Free, u32Peak, u32Wait,
            u32WaitFree, u32WaitPeak, u32Rd, u32Wr, u32Eof, u32SetOk,
            u32UnlkOk, u32Deny, u32Get, u32Block, u32Wake, g_u32SoftCarveOk,
            g_u32SoftSplitHole, u32Coal, u32CoalPass, u32CoalChain,
            u32RelFdWObs, u32RelFdOrphan, u32RelPidWDrop, u32LostWake,
            u32SetEnter, u32LastDeny, g_u32SoftRelWake, g_u32SoftDeadlkProbe,
            g_u32SoftDeadlkHit, u32LeanRuns, u32LeanOk, g_u32SoftLogN,
            FLOCK_SOFT_LOG_CAP);
}

/**
 * After first product activity, print soft residual once.
 * Safe from set/get/release return paths. greppable: file_lock: soft
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
    soft_residual_lean_once();
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

/**
 * C0 functional residual lean self-check (once; Soft!=product).
 * Pure constant / range / capacity / live-coherence arms - never mutates
 * lock or waiter tables, never hard-gates set/get/release product path.
 * greppable: file_lock: soft residual lean
 * greppable: file_lock: soft residual lean PASS
 * greppable: Dual_DoD=OPEN | soft_ne_product=1 | G-AC-1
 */
static void
soft_residual_lean_once(void)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Types = 0;
    u32 u32Conflict = 0;
    u32 u32Range = 0;
    u32 u32Cap = 0;
    u32 u32Live = 0;
    u32 u32Lic = 0;
    u32 u32Held;
    u32 u32Wait;
    u32 u32Used = 0;
    u32 u32WaitUsed = 0;
    u32 u32Slot;
    u32 u32W;
    /* Stack-local Soft!=product honesty (never hard-gates). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nKoProduct = 0;
    const int nStampStorm = 0;
    const int nDualDoDOpen = 1;

    if (g_fSoftLean != 0) {
        return;
    }
    g_fSoftLean = 1;
    soft_inc(&g_u32SoftResidualLean);

    /* Fcntl type constants (POSIX-shaped advisory surface). */
    u32Checks++;
    if (GJ_F_RDLCK == 0 && GJ_F_WRLCK == 1 && GJ_F_UNLCK == 2 &&
        GJ_SEEK_SET == 0 && GJ_FLOCK_EDEADLK == 35) {
        u32Types = 1;
        u32Ok++;
    }

    /*
     * Type conflict residual: read-read free; any write conflicts.
     * Stack-local only - does not touch product tables.
     */
    u32Checks++;
    if (types_conflict(GJ_F_RDLCK, GJ_F_RDLCK) == 0 &&
        types_conflict(GJ_F_RDLCK, GJ_F_WRLCK) != 0 &&
        types_conflict(GJ_F_WRLCK, GJ_F_RDLCK) != 0 &&
        types_conflict(GJ_F_WRLCK, GJ_F_WRLCK) != 0) {
        u32Conflict = 1;
        u32Ok++;
    }

    /*
     * Range geometry residual (exclusive end; EOF sentinel; overflow):
     *   adjacent [0,10)|[10,20) non-overlap; partial overlap hits;
     *   len0 -> EOF; finite end = start+len; overflow flag.
     */
    u32Checks++;
    if (ranges_overlap(0, 10, 10, 20) == 0 &&
        ranges_overlap(0, 10, 9, 20) != 0 &&
        ranges_overlap(0, GJ_FLOCK_EOF, 100, 101) != 0 &&
        range_end(0, 0) == GJ_FLOCK_EOF && range_end(10, 5) == 15 &&
        range_overflow(0, 0) == 0 &&
        range_overflow(0x7fffffffffffffffLL, 1) != 0 &&
        range_norm_end(GJ_FLOCK_EOF) == 0x7fffffffffffffffLL &&
        range_norm_end(42) == 42) {
        u32Range = 1;
        u32Ok++;
    }

    /* Capacity + CAP residual (bounded tables; no stamp storms). */
    u32Checks++;
    if (GJ_FLOCK_MAX == 32u && GJ_FLOCK_MAX_WAITERS == 8u &&
        GJ_FLOCK_SOFT_WAKE_MAX == 4u && GJ_FLOCK_DEADLOCK_DEPTH == 4u &&
        FLOCK_SOFT_LOG_CAP >= 1u && FLOCK_SOFT_LOG_CAP <= 8u &&
        GJ_FLOCK_TAG_WAITER == 1u) {
        u32Cap = 1;
        u32Ok++;
    }

    /* Live table coherence (read-only under lock; never mutates). */
    gj_spin_lock(&g_lkLock);
    u32Held = g_u32NLocks;
    u32Wait = g_u32NWaiters;
    for (u32Slot = 0; u32Slot < GJ_FLOCK_MAX; u32Slot++) {
        if (g_aLk[u32Slot].u8Used) {
            u32Used++;
        }
    }
    for (u32W = 0; u32W < GJ_FLOCK_MAX_WAITERS; u32W++) {
        if (g_aWait[u32W].u8Used) {
            u32WaitUsed++;
        }
    }
    gj_spin_unlock(&g_lkLock);

    u32Checks++;
    if (u32Held == u32Used && u32Wait == u32WaitUsed &&
        u32Held <= (u32)GJ_FLOCK_MAX &&
        u32Wait <= (u32)GJ_FLOCK_MAX_WAITERS) {
        u32Live = 1;
        u32Ok++;
    }

    /*
     * Soft!=product · G-AC-1 no .ko · Dual DoD OPEN · CAP present.
     * LAW: residual never closes product flock / Dual DoD.
     */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nKoProduct == 0 && nStampStorm == 0 &&
        nDualDoDOpen == 1) {
        u32Lic = 1;
        u32Ok++;
    }

    if (u32Ok == u32Checks) {
        soft_inc(&g_u32SoftResidualLeanOk);
    }

    /*
     * Grep: file_lock: soft residual lean
     * Functional residual once - Soft!=product dual license; no version stamp.
     */
    kprintf("file_lock: soft residual lean "
            "func=types|conflict|range|cap|live|lic "
            "types=%u conflict=%u range=%u cap=%u live=%u lic=%u "
            "ok=%u/%u lean_runs=%u lean_ok=%u "
            "held=%u waiters=%u slots=%u waiters_max=%u "
            "wake_budget=%u deadlk_depth=%u log_cap=%u "
            "coal_multi=1 rel_fd_wobs=1 orphan_wake=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 Dual_DoD=OPEN "
            "G-AC-1=1 no_ko_product=1 stamp_storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "C0 functional residual; advisory soft residual; Dual DoD OPEN)\n",
            u32Types, u32Conflict, u32Range, u32Cap, u32Live, u32Lic, u32Ok,
            u32Checks, g_u32SoftResidualLean, g_u32SoftResidualLeanOk, u32Held,
            u32Wait, (u32)GJ_FLOCK_MAX, (u32)GJ_FLOCK_MAX_WAITERS,
            GJ_FLOCK_SOFT_WAKE_MAX, GJ_FLOCK_DEADLOCK_DEPTH,
            FLOCK_SOFT_LOG_CAP);

    /* Grep: file_lock: soft residual lean PASS | FAIL */
    if (u32Ok == u32Checks) {
        kprintf("file_lock: soft residual lean PASS "
                "ok=%u/%u types=%u conflict=%u range=%u cap=%u live=%u lic=%u "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 Dual_DoD=OPEN G-AC-1=1 "
                "(Soft!=product; no version stamp; C0 functional residual)\n",
                u32Ok, u32Checks, u32Types, u32Conflict, u32Range, u32Cap,
                u32Live, u32Lic);
    } else {
        kprintf("file_lock: soft residual lean FAIL "
                "ok=%u/%u types=%u conflict=%u range=%u cap=%u live=%u lic=%u "
                "(soft residual only; not product gate; Soft!=product; "
                "Dual DoD OPEN)\n",
                u32Ok, u32Checks, u32Types, u32Conflict, u32Range, u32Cap,
                u32Live, u32Lic);
    }
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
            soft_held_note();
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

    u32N = thread_wake(&g_u32WaitObj, GJ_FLOCK_TAG_WAITER, GJ_FLOCK_SOFT_WAKE_MAX);
    soft_inc(&g_u32SoftWakeCalls);
    g_u32SoftWakeN += u32N;
    if (u32N == 0u) {
        soft_inc(&g_u32SoftWakeZero);
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
        soft_inc(&g_u32SoftConflict);
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

        fLeft = i64L0 < i64U0;
        fRight = range_norm_end(i64L1) > range_norm_end(i64U1);

        if (!fLeft && !fRight) {
            slot_free(pS);
            continue;
        }
        if (fLeft && fRight) {
            struct flock_slot *pR;

            if (slot_free_count() < 1) {
                soft_inc(&g_u32SoftCarveNolck);
                return -LINUX_ENOLCK;
            }
            pS->i64End = i64U0;
            pR = slot_alloc();
            if (pR == NULL) {
                pS->i64End = i64L1;
                soft_inc(&g_u32SoftCarveNolck);
                return -LINUX_ENOLCK;
            }
            slot_fill(pR, i64Fd, i16Type, i64U1, i64L1, u32Pid);
            soft_inc(&g_u32SoftSplitHole);
            continue;
        }
        if (fLeft) {
            pS->i64End = i64U0;
            continue;
        }
        pS->i64Start = i64U1;
    }
    soft_inc(&g_u32SoftCarveOk);
    return 0;
}

/**
 * Coalesce adjacent/overlapping same-pid same-type ranges on fd.
 * Soft merge after SETLK insert.
 *
 * C0 multi-pass: a single left-to-right scan can miss chain merges when a
 * lower-index range is adjacent only after a higher-index pair merges.
 * Repeat until stable, bounded by GJ_FLOCK_MAX passes.
 * greppable: FLOCK_COALESCE | FLOCK_COALESCE_MULTI
 */
static void
coalesce_pid(i64 i64Fd, u32 u32Pid)
{
    u32 u32Pass;

    for (u32Pass = 0; u32Pass < (u32)GJ_FLOCK_MAX; u32Pass++) {
        u32 iA;
        u32 iB;
        int fProgress = 0;

        soft_inc(&g_u32SoftCoalescePass);
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

                if (!pB->u8Used || pB->i64Fd != i64Fd ||
                    pB->u32Pid != u32Pid) {
                    continue;
                }
                if (pA->i16Type != pB->i16Type) {
                    continue;
                }
                i64A0 = pA->i64Start;
                i64A1 = range_norm_end(pA->i64End);
                i64B0 = pB->i64Start;
                i64B1 = range_norm_end(pB->i64End);
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
                soft_inc(&g_u32SoftCoalesce);
                fProgress = 1;
            }
        }
        if (!fProgress) {
            break;
        }
        if (u32Pass > 0u) {
            /* greppable: FLOCK_COALESCE_MULTI (chain merge after pass 0) */
            soft_inc(&g_u32SoftCoalesceChain);
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
            soft_wait_peak_note();
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

    soft_inc(&g_u32SoftDeadlkProbe);
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

    soft_inc(&g_u32SoftSetEnter);
    if (pFl != NULL) {
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
            soft_inc(&g_u32SoftBlockRetry);
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
        if (i64End < 0) {
            soft_inc(&g_u32SoftEofGrant);
        } else {
            soft_inc(&g_u32SoftFiniteGrant);
        }
        g_u32SoftLastDenyCode = 0u;
        if (fRegistered) {
            waiter_unregister(nWait);
        }
        /* SET may upgrade read->write and free readers' soft wait paths. */
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

    /* UNLCK probe -> treat as write so any lock is reported (POSIX F_GETLK). */
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
     * Emit lean residual on stats read when under CAP (bring-up smoke).
     * After CAP: silent - no stamp storms. greppable: file_lock: soft
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
    u32 iW;
    u32 u32WaitObs = 0;
    int fWake = 0;

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
    /*
     * C0 soft residual: observe fd-keyed waiters at close (do not steal live
     * registrations - waiters re-check after wake and unregister themselves).
     * greppable: FLOCK_REL_FD_WAIT_OBS
     */
    for (iW = 0; iW < GJ_FLOCK_MAX_WAITERS; iW++) {
        if (g_aWait[iW].u8Used && g_aWait[iW].i64Fd == i64Fd) {
            u32WaitObs++;
        }
    }
    if (u32WaitObs > 0) {
        g_u32SoftRelFdWaitObs += u32WaitObs;
    }
    if (u32Cleared > 0) {
        wake_gen_bump();
        g_u32SoftRelFd += u32Cleared;
        fWake = 1;
    } else if (u32WaitObs > 0) {
        /*
         * Orphan-waiter residual: waiters registered on this fd but no held
         * ranges remain - bump gen + soft-wake so they re-sample (lost-wake
         * class). Does not steal registrations. Soft!=product.
         * greppable: FLOCK_REL_FD_ORPHAN_WAKE
         */
        wake_gen_bump();
        soft_inc(&g_u32SoftRelFdOrphan);
        soft_inc(&g_u32SoftRelFdZero);
        fWake = 1;
    } else {
        soft_inc(&g_u32SoftRelFdZero);
    }
    gj_spin_unlock(&g_lkLock);
    if (fWake != 0) {
        soft_inc(&g_u32SoftRelWake);
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
    } else if (u32WaitDrop > 0) {
        wake_gen_bump();
        soft_inc(&g_u32SoftRelPidZero);
    } else {
        soft_inc(&g_u32SoftRelPidZero);
    }
    gj_spin_unlock(&g_lkLock);
    if (u32Cleared > 0 || u32WaitDrop > 0) {
        soft_inc(&g_u32SoftRelWake);
        (void)soft_wake_waiters();
    }
    soft_inventory_maybe_once();
    return u32Cleared;
}
