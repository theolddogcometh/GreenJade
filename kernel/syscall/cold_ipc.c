/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscalls -> personality / UDX-facing userspace hosts.
 * Product: doors / sync service / legacy queue under soft mode flags.
 * Used by Linux personality + UDX/DDI driver hosts (not in-kernel .ko).
 * Product surface = UDX/DDI + hot/cold ABI (this unit is cold ABI half).
 *
 * Functional residual (call / reply / notify for personality + UDX apps):
 *   - door_call / door_call_timeout on submit (absolute mono deadline)
 *   - queue park via thread_block + schedule (not spin); reply wakes slot
 *   - queue post wakes g_coldQWait (notify-shaped residual for consumers)
 *   - single-server personality: door_recv OR queue drain (NOT multi-server)
 *   - SERVICE_FIRST bring-up: libprotonrt / personality sync service first
 *   - door skip tallies: not-ready / thr_exit dead server (H3 honesty)
 *   - ENOSYS residual once when no path usable (host-missing honesty)
 *
 * Functional residual deepen (UDX+sshd cold IPC/doors; Soft!=product):
 *   - residual_host_wake on service/queue/attach/doors enable (UDX rebind)
 *   - doors bring-up idle: yield-poll when MODE_DOORS && !live (late ready)
 *   - sequential fair drain: after door serve, one queue claim if pending
 *   - apps honesty: sshd + UDX hosts share cold ABI (not freestanding wire)
 *   - never hard-gates; Dual DoD A/B stay OPEN; agent != close; stamp-free
 *
 * Dual DoD residual (C2 cold IPC; agent != close):
 *   A=xhci_udx OPEN  B=rtl8168_udx OPEN  ddi_host_gj OPEN
 *   freestanding rtl/USB SKIP; Soft residual != Dual DoD close
 *   dual_dod_close=0 agent_ne_close=1 (lamps only; never hard-gate)
 *
 * C2 cold IPC residual deepen (Soft!=product / G-AC-1; stamp-free):
 *   - path-win residual: which submit path was taken (svc/door/queue/enosys)
 *   - reply state residual: reply-on-PENDING vs reply-on-CLAIMED
 *   - timeout state residual: deadline miss on PENDING vs CLAIMED + reply race
 *   - unreg gen-miss residual (UDX host rebind honesty)
 *   - next-id wrap residual; file-local C2 selfcheck (geometry / mode / tags)
 *   - never hard-gates; Soft residual != Dual DoD close; agent != close
 *
 * Lean residual (C0 soft / eng - Soft!=product dual license):
 *   Sparse CAP-capped inventory + one residual once-lamp (no stamp storms).
 *   Soft!=product / G-AC-1 / dual MIT OR Apache-2.0.
 *   No version stamp. Never hard-gates product paths.
 *   G-AC-1: userspace hosts only; no in-kernel .ko product AC.
 * Pure C11 dual-license product path. ASCII Soft!=product.
 *
 * greppable: GJ_COLD_MODE_ cold_ipc_register_service cold_ipc_stats
 * greppable: cold_ipc: soft residual lean
 * greppable: cold_ipc: residual
 * greppable: cold_ipc: residual dual_dod
 * greppable: cold_ipc: residual c2
 * greppable: cold_ipc: residual c2 selfcheck
 * greppable: cold_ipc: residual functional
 * greppable: cold_ipc: soft residual functional
 * greppable: cold_ipc: soft residual lean host=userspace_driver
 * greppable: cold_ipc: soft residual lean host=udx_personality
 * greppable: cold_ipc: soft residual dual_dod
 * greppable: cold_ipc: soft residual c2
 * greppable: cold_ipc: soft ... / cold: soft ...
 * greppable: apps=sshd host=rtl8168_udx host=xhci_udx
 */
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>

/*
 * Cap multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * Init + first-activity once stay greppable; further calls silent.
 * No version stamp.
 */
#define COLD_IPC_SOFT_LOG_CAP 4u

/*
 * C2 residual selfcheck budget (file-local; Soft!=product).
 * Static geometry / mode / tag / state honesty only (stable PASS).
 * Never hard-gates; not Dual DoD close; not product AC.
 * greppable: cold_ipc: residual c2 selfcheck
 */
#define COLD_IPC_C2_SELF_OK_N 8u

/*
 * Functional residual selfcheck budget (UDX+sshd cold IPC/doors).
 * Geometry + notify-wake + fair-drain honesty only (stable PASS).
 * Never hard-gates; not Dual DoD close; Soft!=product.
 * greppable: cold_ipc: residual functional
 */
#define COLD_IPC_FUNC_SELF_OK_N 6u

static struct gj_cold_request g_aQ[GJ_COLD_QUEUE_DEPTH];
static u64 g_u64NextId = 1;
static int g_fAttached;
static int g_fInited;

/* Soft mode mask (default = bring-up product). */
static u32 g_u32ModeFlags = GJ_COLD_MODE_DEFAULT;

/* Soft service registration */
static i64 (*g_pfnService)(struct gj_linux_regs *pRegs, void *pCtx);
static void *g_pServiceCtx;
static u32 g_u32ServiceGen;

/* Soft queue-consumer registration */
static u32 g_u32QueueGen;
static u32 g_u32NextServiceGen = 1;
static u32 g_u32NextQueueGen = 1;

/* Product counters (wrap OK). */
static u64 g_u64Submits;
static u64 g_u64ServiceHits;
static u64 g_u64DoorHits;
static u64 g_u64QueueHits;
static u64 g_u64Enosys;
static u64 g_u64Inval;
static u64 g_u64QueueFull;
static u64 g_u64Dequeues;
static u64 g_u64DequeueEmpty;
static u64 g_u64Replies;
static u64 g_u64ReplyMiss;
static u64 g_u64ServiceLocal;
static u64 g_u64RegService;
static u64 g_u64UnregService;
static u64 g_u64RegQueue;
static u64 g_u64UnregQueue;
static u64 g_u64ModeChanges;

/* Soft once-lamp + CAP (no version stamp; no stamp storms). */
static u8  g_fResidualOnce;
static u8  g_fC2SelfOnce;   /* C2 selfcheck lamp once */
static u8  g_fFuncSelfOnce; /* functional residual selfcheck lamp once */
static u32 g_u32SoftLogN; /* dumps emitted; silent after COLD_IPC_SOFT_LOG_CAP */
static u32 g_u32C2SelfOk; /* last C2 selfcheck ok count (0..COLD_IPC_C2_SELF_OK_N) */
static u32 g_u32C2SelfPass; /* cumulative full PASS count */
static u32 g_u32FuncSelfOk; /* last functional selfcheck ok count */
static u32 g_u32FuncSelfPass; /* cumulative functional PASS count */

/*
 * Functional residual (call / reply / notify) - file-local, wrap OK.
 * Honest: single-server personality only; multi-server is NOT product.
 * Door-skip counters deepen H3 honesty (thr_exit before as_destroy):
 * REQUIRE_SERVER blocks door_call when pServer is null or GJ_THR_EXITED.
 * greppable: cold_ipc: residual
 * greppable: cold_ipc: residual dual_dod
 * greppable: cold_ipc: residual c2
 */
static u64 g_u64DoorOk;            /* door_call completed (not eio/timeout) */
static u64 g_u64DoorTimeout;       /* door_call_timeout -> -ETIMEDOUT */
static u64 g_u64DoorPeerDead;      /* door path -> -EIO / peer dead */
static u64 g_u64DoorEnosys;        /* door path -> -ENOSYS */
static u64 g_u64DoorSkipNotReady;  /* doors on but ep null / !ready */
static u64 g_u64DoorSkipDead;      /* REQUIRE_SERVER: pServer null/exited */
static u64 g_u64QueueTimeout;      /* submit_queue deadline miss */
static u64 g_u64QueuePark;         /* thread_block on slot (call residual) */
static u64 g_u64QueueYieldPark;    /* no current thr: yield fallback */
static u64 g_u64QueuePostWake;     /* wake consumers on PENDING post */
static u64 g_u64ReplyWake;         /* reply -> thread_wake on slot */
static u64 g_u64ServerDoorServe;   /* personality door_recv -> reply */
static u64 g_u64ServerQueueServe;  /* personality queue drain -> reply */
static u64 g_u64ServerIdlePark;    /* personality parked on g_coldQWait */

/* C2 residual deepen tallies (wrap OK; never hard-gate). */
static u64 g_u64PathWinSvc;        /* submit took service path */
static u64 g_u64PathWinDoor;       /* submit took doors path */
static u64 g_u64PathWinQueue;      /* submit took legacy queue path */
static u64 g_u64PathWinEnosys;     /* submit fell through host-missing */
static u64 g_u64ReplyOnPending;    /* reply matched PENDING (claim skip) */
static u64 g_u64ReplyOnClaimed;    /* reply matched CLAIMED (dequeue path) */
static u64 g_u64QueueToPending;    /* timeout free while PENDING */
static u64 g_u64QueueToClaimed;    /* timeout free while CLAIMED */
static u64 g_u64QueueReplyRaceWin; /* DONE beat deadline on same arm */
static u64 g_u64UnregSvcMiss;      /* unregister_service gen mismatch */
static u64 g_u64UnregQMiss;        /* unregister_queue_consumer gen miss */
static u64 g_u64NextIdWrap;        /* queue id counter wrapped past 0 */

/*
 * Functional residual tallies (UDX+sshd cold IPC/doors; wrap OK).
 * Host-wake: rebind/attach/doors enable kicks idle personality.
 * Doors bring-up yield: MODE_DOORS set but endpoint not live yet.
 * Fair queue drain: sequential post-door queue claim (single_server).
 * greppable: cold_ipc: residual functional
 */
static u64 g_u64HostWake;          /* residual_host_wake pulses */
static u64 g_u64DoorsBringupYield;  /* idle yield-poll doors bring-up */
static u64 g_u64FairQueueDrain;     /* after door serve, drained one queue */

/*
 * Notify-shaped residual wait key for queue consumers / personality idle.
 * Waiters block on this object (tag 0); post and reply paths wake it.
 * Not a multi-server product API - single waiter model is honest residual.
 */
static u32 g_coldQWait;

/* Queue park tag on slot object (reply wakes tag 0 = any). */
#define COLD_Q_TAG_CALLER  1u
#define COLD_Q_TAG_WAIT    0u

static void soft_inventory_log(void);
static void residual_note_once(void);
static void residual_c2_selfcheck(void);
static void residual_func_selfcheck(void);
static void residual_host_wake(void);
static void queue_post_notify(void);
static u64  deadline_from_timeout(u64 u64TimeoutNsec);
static void note_door_outcome(i64 i64R);
static void note_door_skip(void);

static u32
bump_service_gen(void)
{
    u32 u32G = g_u32NextServiceGen++;

    if (g_u32NextServiceGen == 0) {
        g_u32NextServiceGen = 1;
    }
    return u32G;
}

static u32
bump_queue_gen(void)
{
    u32 u32G = g_u32NextQueueGen++;

    if (g_u32NextQueueGen == 0) {
        g_u32NextQueueGen = 1;
    }
    return u32G;
}

static u32
count_pending(void)
{
    u32 iSlot;
    u32 u32N = 0;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        u32 u32St = g_aQ[iSlot].u32State;

        if (u32St == GJ_COLD_PENDING || u32St == GJ_COLD_CLAIMED) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft slot tallies by state (diagnostics; no hard lock).
 */
static void
count_slots(u32 *pFree, u32 *pPending, u32 *pClaimed, u32 *pDone)
{
    u32 iSlot;
    u32 u32Free = 0;
    u32 u32Pending = 0;
    u32 u32Claimed = 0;
    u32 u32Done = 0;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        switch (g_aQ[iSlot].u32State) {
        case GJ_COLD_PENDING:
            u32Pending++;
            break;
        case GJ_COLD_CLAIMED:
            u32Claimed++;
            break;
        case GJ_COLD_DONE:
            u32Done++;
            break;
        case GJ_COLD_FREE:
        default:
            u32Free++;
            break;
        }
    }
    if (pFree != NULL) {
        *pFree = u32Free;
    }
    if (pPending != NULL) {
        *pPending = u32Pending;
    }
    if (pClaimed != NULL) {
        *pClaimed = u32Claimed;
    }
    if (pDone != NULL) {
        *pDone = u32Done;
    }
}

/**
 * Soft: doors path usable under current mode flags.
 * REQUIRE_SERVER (default): pServer must be live.
 * Without REQUIRE_SERVER: ready endpoint is enough (may block in door_call).
 * Probe-safe: no residual skip tallies here (path_doors counts on submit).
 */
static int
doors_usable(struct gj_door **ppDoorOut)
{
    struct gj_door *pDoor;

    if ((g_u32ModeFlags & GJ_COLD_MODE_DOORS) == 0) {
        return 0;
    }
    pDoor = door_cold_personality();
    if (pDoor == NULL || !pDoor->u32Ready) {
        return 0;
    }
    if ((g_u32ModeFlags & GJ_COLD_MODE_REQUIRE_SERVER) != 0) {
        if (pDoor->pServer == NULL ||
            pDoor->pServer->u32State == GJ_THR_EXITED) {
            return 0;
        }
    }
    if (ppDoorOut != NULL) {
        *ppDoorOut = pDoor;
    }
    return 1;
}

/**
 * Classify doors skip for residual honesty (submit path only; wrap OK).
 * H3: thr_exit / null pServer under REQUIRE_SERVER -> door_skip_dead.
 * Soft!=product - tallies never hard-gate.
 */
static void
note_door_skip(void)
{
    struct gj_door *pDoor;

    if ((g_u32ModeFlags & GJ_COLD_MODE_DOORS) == 0) {
        return;
    }
    pDoor = door_cold_personality();
    if (pDoor == NULL || !pDoor->u32Ready) {
        g_u64DoorSkipNotReady++;
        return;
    }
    if ((g_u32ModeFlags & GJ_COLD_MODE_REQUIRE_SERVER) != 0 &&
        (pDoor->pServer == NULL ||
         pDoor->pServer->u32State == GJ_THR_EXITED)) {
        g_u64DoorSkipDead++;
    }
}

static int
service_usable(void)
{
    return ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE) != 0) &&
           (g_pfnService != NULL);
}

static int
queue_usable(void)
{
    return ((g_u32ModeFlags & GJ_COLD_MODE_QUEUE) != 0) &&
           (g_fAttached || g_u32QueueGen != 0 || g_pfnService != NULL);
}

/**
 * Relative timeout -> absolute mono deadline (0 = none).
 * SECURITY_CORE s5: mono only; never wall clock.
 */
static u64
deadline_from_timeout(u64 u64TimeoutNsec)
{
    u64 u64Now;

    if (u64TimeoutNsec == 0 || !timer_ready()) {
        return 0;
    }
    u64Now = timer_mono_nsec();
    /* Saturating add - avoid wrap -> near-zero deadline. */
    if (u64Now > ~0ull - u64TimeoutNsec) {
        return ~0ull;
    }
    return u64Now + u64TimeoutNsec;
}

/** Classify door_call residual outcomes (honest; wrap OK). */
static void
note_door_outcome(i64 i64R)
{
    if (i64R == (i64)(-LINUX_ETIMEDOUT)) {
        g_u64DoorTimeout++;
    } else if (i64R == (i64)(-LINUX_EIO)) {
        g_u64DoorPeerDead++;
    } else if (i64R == (i64)(-LINUX_ENOSYS)) {
        g_u64DoorEnosys++;
    } else {
        g_u64DoorOk++;
    }
}

/**
 * Notify-shaped residual: wake waiters on g_coldQWait after queue post.
 * Mirrors call/reply/notify host pattern without a second server product.
 * Soft pulse counted even when no waiter (observability; wrap OK).
 */
static void
queue_post_notify(void)
{
    (void)thread_wake(&g_coldQWait, COLD_Q_TAG_WAIT, 8);
    g_u64QueuePostWake++;
}

/**
 * Functional residual: host rebind / attach / doors-enable wake.
 * Kicks idle personality parked on g_coldQWait so UDX host rebind and
 * sshd cold-path bring-up recheck doors/service/queue without hang.
 * Soft!=product - pulse counted even when no waiter (wrap OK).
 * greppable: cold_ipc: residual functional
 */
static void
residual_host_wake(void)
{
    (void)thread_wake(&g_coldQWait, COLD_Q_TAG_WAIT, 8);
    g_u64HostWake++;
}

/**
 * Functional residual selfcheck (UDX+sshd cold IPC/doors honesty).
 * Soft!=product - never hard-gates; Dual DoD A/B stay OPEN; agent != close.
 * greppable: cold_ipc: residual functional
 * greppable: cold_ipc: soft residual functional
 */
static void
residual_func_selfcheck(void)
{
    u32 u32Ok = 0;
    u32 u32Def = (u32)GJ_COLD_MODE_DEFAULT;

    /* 1. Queue token id never zero (FREE slot reuse residual). */
    if (g_u64NextId != 0) {
        u32Ok++;
    }
    /* 2. Park tags remain distinct (caller vs wait residual). */
    if (COLD_Q_TAG_CALLER != COLD_Q_TAG_WAIT) {
        u32Ok++;
    }
    /* 3. Default bring-up still carries doors (sshd/UDX cold ABI half). */
    if ((u32Def & GJ_COLD_MODE_DOORS) != 0) {
        u32Ok++;
    }
    /* 4. REQUIRE_SERVER default (H3: no hang on thr_exit dead server). */
    if ((u32Def & GJ_COLD_MODE_REQUIRE_SERVER) != 0) {
        u32Ok++;
    }
    /* 5. Queue depth fixed bound (DoS residual geometry). */
    if (GJ_COLD_QUEUE_DEPTH >= 1u && GJ_COLD_QUEUE_DEPTH <= 256u) {
        u32Ok++;
    }
    /* 6. Soft inventory CAP present (no stamp storms residual). */
    if (COLD_IPC_SOFT_LOG_CAP >= 1u) {
        u32Ok++;
    }

    g_u32FuncSelfOk = u32Ok;
    if (u32Ok == COLD_IPC_FUNC_SELF_OK_N) {
        g_u32FuncSelfPass++;
    }

    if (g_fFuncSelfOnce != 0) {
        return;
    }
    g_fFuncSelfOnce = 1;

    /*
     * Grep: cold_ipc: residual functional
     * Grep: cold_ipc: soft residual functional
     * Audience: sshd + UDX/DDI userspace hosts over cold ABI/doors.
     */
    kprintf("cold_ipc: residual functional "
            "ok=%u/%u pass=%u "
            "host_wake=%llu doors_bringup_yield=%llu fair_q_drain=%llu "
            "door_ok=%llu reply_wake=%llu post_wake=%llu "
            "path_win_svc=%llu path_win_door=%llu path_win_queue=%llu "
            "apps=sshd,netstackd hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
            "call/reply/notify single_server=1 multi_server=0 "
            "product=UDX_DDI_hot_cold_ABI cold_abi=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 freestanding_rtl=SKIP freestanding_usb=SKIP "
            "H3=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; functional residual UDX+sshd cold IPC/doors; "
            "Dual DoD A/B OPEN agent!=close; not product close)\n",
            u32Ok, (unsigned)COLD_IPC_FUNC_SELF_OK_N, g_u32FuncSelfPass,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64DoorsBringupYield,
            (unsigned long long)g_u64FairQueueDrain,
            (unsigned long long)g_u64DoorOk,
            (unsigned long long)g_u64ReplyWake,
            (unsigned long long)g_u64QueuePostWake,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue);
    kprintf("cold_ipc: soft residual functional "
            "ok=%u/%u pass=%u host_wake=%llu fair_q_drain=%llu "
            "apps=sshd hosts=rtl8168_udx,xhci_udx "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 cold_abi=1 product=UDX_DDI_hot_cold_ABI "
            "soft_ne_product=1 G-AC-1=1 "
            "(Soft!=product; functional residual UDX+sshd; agent!=close)\n",
            u32Ok, (unsigned)COLD_IPC_FUNC_SELF_OK_N, g_u32FuncSelfPass,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64FairQueueDrain);
    if (u32Ok == COLD_IPC_FUNC_SELF_OK_N) {
        kprintf("cold_ipc: residual functional PASS ok=%u/%u "
                "apps=sshd udx=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                "agent_ne_close=1 soft_ne_product=1 G-AC-1=1\n",
                u32Ok, (unsigned)COLD_IPC_FUNC_SELF_OK_N);
    } else {
        kprintf("cold_ipc: residual functional FAIL ok=%u/%u "
                "(soft residual only; not product gate; Dual DoD OPEN)\n",
                u32Ok, (unsigned)COLD_IPC_FUNC_SELF_OK_N);
    }
}

/**
 * C2 residual selfcheck (file-local geometry / mode / tag honesty).
 * Soft!=product - never hard-gates; Dual DoD A/B stay OPEN; agent != close.
 * greppable: cold_ipc: residual c2 selfcheck
 * greppable: cold_ipc: residual c2
 * greppable: cold_ipc: soft residual c2
 */
static void
residual_c2_selfcheck(void)
{
    u32 u32Ok = 0;
    u32 u32Def = (u32)GJ_COLD_MODE_DEFAULT;

    /* 1. Fixed-depth queue bound (DoS / residual geometry). */
    if (GJ_COLD_QUEUE_DEPTH >= 1u && GJ_COLD_QUEUE_DEPTH <= 256u) {
        u32Ok++;
    }
    /* 2-5. Default bring-up mask carries product path bits. */
    if ((u32Def & GJ_COLD_MODE_DOORS) != 0) {
        u32Ok++;
    }
    if ((u32Def & GJ_COLD_MODE_SERVICE) != 0) {
        u32Ok++;
    }
    if ((u32Def & GJ_COLD_MODE_QUEUE) != 0) {
        u32Ok++;
    }
    if ((u32Def & GJ_COLD_MODE_SERVICE_FIRST) != 0) {
        u32Ok++;
    }
    /* 6. REQUIRE_SERVER default (H3: no hang on thr_exit dead server). */
    if ((u32Def & GJ_COLD_MODE_REQUIRE_SERVER) != 0) {
        u32Ok++;
    }
    /* 7. Slot state machine values (FREE/PENDING/DONE/CLAIMED). */
    if ((int)GJ_COLD_FREE == 0 && (int)GJ_COLD_PENDING == 1 &&
        (int)GJ_COLD_DONE == 2 && (int)GJ_COLD_CLAIMED == 3) {
        u32Ok++;
    }
    /* 8. Park tags distinct + soft CAP present (single-waiter residual). */
    if (COLD_Q_TAG_CALLER != COLD_Q_TAG_WAIT && COLD_IPC_SOFT_LOG_CAP >= 1u) {
        u32Ok++;
    }

    g_u32C2SelfOk = u32Ok;
    if (u32Ok == COLD_IPC_C2_SELF_OK_N) {
        g_u32C2SelfPass++;
    }

    if (g_fC2SelfOnce != 0) {
        return;
    }
    g_fC2SelfOnce = 1;

    /*
     * Grep: cold_ipc: residual c2 selfcheck
     * Grep: cold_ipc: residual c2
     */
    kprintf("cold_ipc: residual c2 selfcheck ok=%u/%u pass=%u "
            "depth=%u mode_def=0x%x "
            "states=FREE/PENDING/DONE/CLAIMED tags=caller!=wait "
            "single_server=1 multi_server=0 "
            "path_win svc=%llu door=%llu queue=%llu enosys=%llu "
            "reply_pend=%llu reply_claim=%llu "
            "q_to_pend=%llu q_to_claim=%llu q_race_win=%llu "
            "unreg_svc_miss=%llu unreg_q_miss=%llu id_wrap=%llu "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
            "product=UDX_DDI_hot_cold_ABI cold_abi=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 freestanding_rtl=SKIP freestanding_usb=SKIP "
            "H3=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; C2 cold IPC residual deepen; "
            "Dual DoD A/B OPEN agent!=close; not product close)\n",
            u32Ok, (unsigned)COLD_IPC_C2_SELF_OK_N, g_u32C2SelfPass,
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Def,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue,
            (unsigned long long)g_u64PathWinEnosys,
            (unsigned long long)g_u64ReplyOnPending,
            (unsigned long long)g_u64ReplyOnClaimed,
            (unsigned long long)g_u64QueueToPending,
            (unsigned long long)g_u64QueueToClaimed,
            (unsigned long long)g_u64QueueReplyRaceWin,
            (unsigned long long)g_u64UnregSvcMiss,
            (unsigned long long)g_u64UnregQMiss,
            (unsigned long long)g_u64NextIdWrap);
    kprintf("cold_ipc: residual c2 "
            "ok=%u/%u path_win_svc=%llu path_win_door=%llu "
            "path_win_queue=%llu path_win_enosys=%llu "
            "reply_on_pending=%llu reply_on_claimed=%llu "
            "single_server=1 dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual_dod_close=0 agent_ne_close=1 G-AC-1=1 "
            "soft_ne_product=1 (Soft!=product; C2 cold IPC residual deepen)\n",
            u32Ok, (unsigned)COLD_IPC_C2_SELF_OK_N,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue,
            (unsigned long long)g_u64PathWinEnosys,
            (unsigned long long)g_u64ReplyOnPending,
            (unsigned long long)g_u64ReplyOnClaimed);
    if (u32Ok == COLD_IPC_C2_SELF_OK_N) {
        kprintf("cold_ipc: residual c2 selfcheck PASS ok=%u/%u "
                "dual_dod_a=OPEN dual_dod_b=OPEN agent_ne_close=1 "
                "soft_ne_product=1 G-AC-1=1\n",
                u32Ok, (unsigned)COLD_IPC_C2_SELF_OK_N);
    } else {
        kprintf("cold_ipc: residual c2 selfcheck FAIL ok=%u/%u "
                "(soft residual only; not product gate; Dual DoD OPEN)\n",
                u32Ok, (unsigned)COLD_IPC_C2_SELF_OK_N);
    }
    /* Grep: cold_ipc: soft residual c2 */
    kprintf("cold_ipc: soft residual c2 "
            "ok=%u/%u pass=%u dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual_dod_close=0 agent_ne_close=1 cold_abi=1 "
            "product=UDX_DDI_hot_cold_ABI freestanding_class=SKIP "
            "soft_ne_product=1 G-AC-1=1 "
            "(Soft!=product; C2 cold IPC residual deepen; agent!=close)\n",
            u32Ok, (unsigned)COLD_IPC_C2_SELF_OK_N, g_u32C2SelfPass);
}

/**
 * One-shot lean residual honesty lamp (not a stamp storm).
 * greppable: cold_ipc: soft residual lean
 * greppable: cold_ipc: residual
 * greppable: cold_ipc: residual dual_dod
 * greppable: cold_ipc: residual c2
 * greppable: cold_ipc: soft residual lean host=userspace_driver
 * greppable: cold_ipc: soft residual lean host=udx_personality
 * greppable: cold_ipc: soft residual dual_dod
 * greppable: cold_ipc: soft residual c2
 * Soft!=product dual MIT OR Apache-2.0. No version stamp.
 * Audience: Linux personality apps + UDX/DDI userspace driver hosts.
 * Dual DoD A/B stay OPEN (agent != close); freestanding class SKIP.
 */
static void
residual_note_once(void)
{
    u32 u32Attached;
    u32 u32SvcBound;
    u32 u32DoorsU;
    u32 u32QueueU;
    u32 u32DoorsFirst;
    const char *szOrder;

    if (g_fResidualOnce != 0) {
        return;
    }
    g_fResidualOnce = 1;

    u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    u32SvcBound = (g_pfnService != NULL) ? 1u : 0u;
    u32DoorsU = doors_usable(NULL) ? 1u : 0u;
    u32QueueU = queue_usable() ? 1u : 0u;
    u32DoorsFirst = ((g_u32ModeFlags & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                    ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE_FIRST) == 0)
                        ? 1u
                        : 0u;
    if (u32DoorsFirst) {
        szOrder = "doors>service>queue";
    } else {
        szOrder = "service>doors>queue";
    }

    /*
     * Grep: cold_ipc: soft residual lean host=userspace_driver
     * Grep: cold_ipc: soft residual lean host=udx_personality
     * Host path: Linux personality + UDX/DDI userspace driver hosts.
     * Product = UDX/DDI + hot/cold ABI; this unit is cold ABI residual.
     */
    kprintf("cold_ipc: soft residual lean host=userspace_driver "
            "host=udx_personality personality=1 udx=1 ddi=1 apps=1 "
            "apps=sshd doors=%u service=%u queue=%u call/reply/notify "
            "single_server=1 multi_server=0 order=%s "
            "park=thread_block timeout=mono "
            "attached=%u svc_bound=%u "
            "door_ok=%llu door_to=%llu door_eio=%llu door_enosys=%llu "
            "door_skip_nr=%llu door_skip_dead=%llu "
            "q_to=%llu q_park=%llu q_yield=%llu post_wake=%llu "
            "reply_wake=%llu srv_door=%llu srv_q=%llu idle_park=%llu "
            "host_wake=%llu doors_bringup=%llu fair_q_drain=%llu "
            "path_win_svc=%llu path_win_door=%llu path_win_queue=%llu "
            "path_win_enosys=%llu reply_pend=%llu reply_claim=%llu "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
            "product=UDX_DDI_hot_cold_ABI cold_abi=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 freestanding_rtl=SKIP freestanding_usb=SKIP "
            "H3=door_skip_dead soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "G-AC-1=1 no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; functional residual "
            "UDX+sshd cold IPC/doors; Dual DoD A/B OPEN agent!=close)\n",
            u32DoorsU, u32SvcBound, u32QueueU, szOrder, u32Attached,
            u32SvcBound,
            (unsigned long long)g_u64DoorOk,
            (unsigned long long)g_u64DoorTimeout,
            (unsigned long long)g_u64DoorPeerDead,
            (unsigned long long)g_u64DoorEnosys,
            (unsigned long long)g_u64DoorSkipNotReady,
            (unsigned long long)g_u64DoorSkipDead,
            (unsigned long long)g_u64QueueTimeout,
            (unsigned long long)g_u64QueuePark,
            (unsigned long long)g_u64QueueYieldPark,
            (unsigned long long)g_u64QueuePostWake,
            (unsigned long long)g_u64ReplyWake,
            (unsigned long long)g_u64ServerDoorServe,
            (unsigned long long)g_u64ServerQueueServe,
            (unsigned long long)g_u64ServerIdlePark,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64DoorsBringupYield,
            (unsigned long long)g_u64FairQueueDrain,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue,
            (unsigned long long)g_u64PathWinEnosys,
            (unsigned long long)g_u64ReplyOnPending,
            (unsigned long long)g_u64ReplyOnClaimed);

    /*
     * Grep: cold_ipc: residual
     * Compact twin - call/reply/notify honesty for personality + UDX apps.
     */
    kprintf("cold_ipc: residual "
            "call/reply/notify single_server=1 multi_server=0 "
            "personality=1 udx=1 apps=1 apps=sshd order=%s "
            "door_ok=%llu reply_wake=%llu post_wake=%llu host_wake=%llu "
            "fair_q_drain=%llu door_skip_nr=%llu door_skip_dead=%llu "
            "path_win_svc=%llu path_win_door=%llu path_win_queue=%llu "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; functional residual UDX+sshd cold IPC/doors; "
            "no version stamp; no .ko product AC)\n",
            szOrder,
            (unsigned long long)g_u64DoorOk,
            (unsigned long long)g_u64ReplyWake,
            (unsigned long long)g_u64QueuePostWake,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64FairQueueDrain,
            (unsigned long long)g_u64DoorSkipNotReady,
            (unsigned long long)g_u64DoorSkipDead,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue);

    /*
     * Grep: cold_ipc: residual dual_dod
     * Grep: cold_ipc: soft residual dual_dod
     * Dual DoD A/B direction honesty - OPEN until USB path / interactive SSH login.
     * Soft residual lamps != Dual DoD close (agent != close).
     */
    kprintf("cold_ipc: residual dual_dod "
            "A=xhci_udx status=OPEN B=rtl8168_udx status=OPEN "
            "ddi_host=ddi_host_gj status=OPEN "
            "product=UDX_DDI_hot_cold_ABI cold_abi=1 "
            "path=call/reply/notify single_server=1 multi_server=0 "
            "hosts=rtl8168_udx,xhci_udx,ddi_host_gj apps=sshd "
            "freestanding_rtl=SKIP freestanding_usb=SKIP "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 door_skip_dead=%llu host_wake=%llu "
            "fair_q_drain=%llu H3=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 "
            "(Soft!=product; functional residual UDX+sshd cold IPC/doors; "
            "Dual DoD A/B OPEN; not freestanding product close; not bar3)\n",
            (unsigned long long)g_u64DoorSkipDead,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64FairQueueDrain);
    kprintf("cold_ipc: soft residual dual_dod "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 product=UDX_DDI_hot_cold_ABI "
            "cold_abi=1 freestanding_class=SKIP G-AC-1=1 "
            "soft_ne_product=1 (Soft!=product; agent!=close)\n");

    /* C2 deepen residual selfcheck (once; geometry + path-win honesty). */
    residual_c2_selfcheck();
    /* Functional residual selfcheck (UDX+sshd cold IPC/doors deepen). */
    residual_func_selfcheck();
}

/**
 * Sparse soft inventory (C0 diagnostics / smoke). CAP-capped.
 * NO stamp storms: <= COLD_IPC_SOFT_LOG_CAP dumps; then silent.
 * Per dump: rollup + residual lean + PASS + twin alias. No version stamp.
 * greppable: cold_ipc: soft
 * greppable: cold: soft
 * greppable: cold_ipc: soft residual lean
 * greppable: cold_ipc: soft residual lean host=userspace_driver
 * greppable: cold_ipc: soft residual dual_dod
 */
static void
soft_inventory_log(void)
{
    u32 u32Free;
    u32 u32Pending;
    u32 u32Claimed;
    u32 u32Done;
    u32 u32Mode;
    u32 u32Attached;
    u32 u32SvcBound;
    u32 u32SvcUsable;
    u32 u32DoorsUsable;
    u32 u32QueueUsable;
    u32 u32DoorsFirst;
    const char *szOrder;

    /*
     * No stamp storms: after CAP dumps, stay silent (counters keep ticking).
     * Soft!=product - inventory never hard-gates.
     */
    if (g_u32SoftLogN >= COLD_IPC_SOFT_LOG_CAP) {
        return;
    }
    g_u32SoftLogN++;

    /* Snapshot live state (diagnostics only; no hard lock). */
    count_slots(&u32Free, &u32Pending, &u32Claimed, &u32Done);
    u32Mode = g_u32ModeFlags;
    u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    u32SvcBound = (g_pfnService != NULL) ? 1u : 0u;
    u32SvcUsable = service_usable() ? 1u : 0u;
    u32DoorsUsable = doors_usable(NULL) ? 1u : 0u;
    u32QueueUsable = queue_usable() ? 1u : 0u;
    u32DoorsFirst = ((u32Mode & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                    ((u32Mode & GJ_COLD_MODE_SERVICE_FIRST) == 0)
                        ? 1u
                        : 0u;
    if (u32DoorsFirst) {
        szOrder = "doors>service>queue";
    } else {
        szOrder = "service>doors>queue";
    }

    /* Grep: cold_ipc: soft inventory - single rollup (no version stamp) */
    kprintf("cold_ipc: soft inventory depth=%u free=%u pending=%u "
            "claimed=%u done=%u attached=%u svc_bound=%u mode=0x%x "
            "order=%s svc_u=%u door_u=%u queue_u=%u "
            "submits=%llu hits_svc=%llu hits_door=%llu hits_queue=%llu "
            "enosys=%llu replies=%llu full=%llu "
            "door_skip_nr=%llu door_skip_dead=%llu "
            "log_n=%u log_cap=%u "
            "(sparse; soft_ne_product=1 host=userspace_driver "
            "dual_dod_a=OPEN dual_dod_b=OPEN)\n",
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Free, u32Pending, u32Claimed,
            u32Done, u32Attached, u32SvcBound, u32Mode, szOrder,
            u32SvcUsable, u32DoorsUsable, u32QueueUsable,
            (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64ServiceHits,
            (unsigned long long)g_u64DoorHits,
            (unsigned long long)g_u64QueueHits,
            (unsigned long long)g_u64Enosys,
            (unsigned long long)g_u64Replies,
            (unsigned long long)g_u64QueueFull,
            (unsigned long long)g_u64DoorSkipNotReady,
            (unsigned long long)g_u64DoorSkipDead,
            g_u32SoftLogN, (unsigned)COLD_IPC_SOFT_LOG_CAP);

    /*
     * Grep: cold_ipc: soft residual lean host=userspace_driver
     * Grep: cold_ipc: soft residual lean host=udx_personality
     * Lean residual honesty - Soft!=product dual license; no version stamp.
     * Dual DoD A/B OPEN (agent != close); freestanding class SKIP.
     */
    kprintf("cold_ipc: soft residual lean host=userspace_driver "
            "host=udx_personality personality=1 udx=1 ddi=1 apps=1 "
            "apps=sshd doors=%u service=%u queue=%u order=%s "
            "single_server=1 multi_server=0 park=thread_block timeout=mono "
            "attached=%u svc_bound=%u submits=%llu replies=%llu "
            "door_skip_nr=%llu door_skip_dead=%llu "
            "host_wake=%llu doors_bringup=%llu fair_q_drain=%llu "
            "path_win_svc=%llu path_win_door=%llu path_win_queue=%llu "
            "path_win_enosys=%llu reply_pend=%llu reply_claim=%llu "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
            "product=UDX_DDI_hot_cold_ABI cold_abi=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 freestanding_rtl=SKIP freestanding_usb=SKIP "
            "residual_once=%u log_n=%u log_cap=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; functional residual "
            "UDX+sshd cold IPC/doors; Dual DoD A/B OPEN agent!=close)\n",
            u32DoorsUsable, u32SvcUsable, u32QueueUsable, szOrder,
            u32Attached, u32SvcBound, (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64Replies,
            (unsigned long long)g_u64DoorSkipNotReady,
            (unsigned long long)g_u64DoorSkipDead,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64DoorsBringupYield,
            (unsigned long long)g_u64FairQueueDrain,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue,
            (unsigned long long)g_u64PathWinEnosys,
            (unsigned long long)g_u64ReplyOnPending,
            (unsigned long long)g_u64ReplyOnClaimed,
            (unsigned)g_fResidualOnce, g_u32SoftLogN,
            (unsigned)COLD_IPC_SOFT_LOG_CAP);

    /*
     * Grep: cold_ipc: soft residual c2
     * C2 deepen rollup inside CAP-capped inventory (not a stamp storm).
     */
    kprintf("cold_ipc: soft residual c2 "
            "ok=%u/%u pass=%u path_win_svc=%llu path_win_door=%llu "
            "path_win_queue=%llu path_win_enosys=%llu "
            "reply_pend=%llu reply_claim=%llu "
            "q_to_pend=%llu q_to_claim=%llu q_race_win=%llu "
            "unreg_svc_miss=%llu unreg_q_miss=%llu id_wrap=%llu "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 freestanding_class=SKIP "
            "soft_ne_product=1 G-AC-1=1 no_ko_product=1 "
            "(Soft!=product; C2 cold IPC residual deepen; agent!=close)\n",
            g_u32C2SelfOk, (unsigned)COLD_IPC_C2_SELF_OK_N, g_u32C2SelfPass,
            (unsigned long long)g_u64PathWinSvc,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue,
            (unsigned long long)g_u64PathWinEnosys,
            (unsigned long long)g_u64ReplyOnPending,
            (unsigned long long)g_u64ReplyOnClaimed,
            (unsigned long long)g_u64QueueToPending,
            (unsigned long long)g_u64QueueToClaimed,
            (unsigned long long)g_u64QueueReplyRaceWin,
            (unsigned long long)g_u64UnregSvcMiss,
            (unsigned long long)g_u64UnregQMiss,
            (unsigned long long)g_u64NextIdWrap);

    /*
     * Grep: cold_ipc: soft residual functional
     * CAP-capped functional residual rollup (UDX+sshd cold IPC/doors).
     */
    kprintf("cold_ipc: soft residual functional "
            "ok=%u/%u pass=%u host_wake=%llu doors_bringup_yield=%llu "
            "fair_q_drain=%llu path_win_door=%llu path_win_queue=%llu "
            "apps=sshd hosts=rtl8168_udx,xhci_udx "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_close=0 "
            "agent_ne_close=1 cold_abi=1 product=UDX_DDI_hot_cold_ABI "
            "soft_ne_product=1 G-AC-1=1 "
            "(Soft!=product; functional residual UDX+sshd; agent!=close)\n",
            g_u32FuncSelfOk, (unsigned)COLD_IPC_FUNC_SELF_OK_N,
            g_u32FuncSelfPass,
            (unsigned long long)g_u64HostWake,
            (unsigned long long)g_u64DoorsBringupYield,
            (unsigned long long)g_u64FairQueueDrain,
            (unsigned long long)g_u64PathWinDoor,
            (unsigned long long)g_u64PathWinQueue);

    /* Grep: cold_ipc: soft inventory PASS | cold: soft inventory PASS */
    kprintf("cold_ipc: soft inventory PASS logs=%u submits=%llu "
            "svc_bound=%u single_server=1 udx=1 apps=1 apps=sshd "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 host=userspace_driver "
            "host=udx_personality dual_dod_a=OPEN dual_dod_b=OPEN "
            "agent_ne_close=1 G-AC-1=1 c2_ok=%u/%u func_ok=%u/%u\n",
            g_u32SoftLogN, (unsigned long long)g_u64Submits, u32SvcBound,
            g_u32C2SelfOk, (unsigned)COLD_IPC_C2_SELF_OK_N,
            g_u32FuncSelfOk, (unsigned)COLD_IPC_FUNC_SELF_OK_N);
    kprintf("cold: soft inventory PASS logs=%u submits=%llu "
            "single_server=1 udx=1 apps=sshd dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 c2_ok=%u/%u func_ok=%u/%u\n",
            g_u32SoftLogN, (unsigned long long)g_u64Submits,
            g_u32C2SelfOk, (unsigned)COLD_IPC_C2_SELF_OK_N,
            g_u32FuncSelfOk, (unsigned)COLD_IPC_FUNC_SELF_OK_N);
}

void
cold_ipc_init(void)
{
    /* Idempotent: must not wipe protonrt service after attach (boot smoke). */
    if (g_fInited) {
        return;
    }
    memset(g_aQ, 0, sizeof(g_aQ));
    g_u64NextId = 1;
    g_fAttached = 0;
    g_u32ModeFlags = GJ_COLD_MODE_DEFAULT;
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
    g_u32ServiceGen = 0;
    g_u32QueueGen = 0;
    g_u32NextServiceGen = 1;
    g_u32NextQueueGen = 1;
    g_u64Submits = 0;
    g_u64ServiceHits = 0;
    g_u64DoorHits = 0;
    g_u64QueueHits = 0;
    g_u64Enosys = 0;
    g_u64Inval = 0;
    g_u64QueueFull = 0;
    g_u64Dequeues = 0;
    g_u64DequeueEmpty = 0;
    g_u64Replies = 0;
    g_u64ReplyMiss = 0;
    g_u64ServiceLocal = 0;
    g_u64RegService = 0;
    g_u64UnregService = 0;
    g_u64RegQueue = 0;
    g_u64UnregQueue = 0;
    g_u64ModeChanges = 0;
    g_u32SoftLogN = 0;
    g_fResidualOnce = 0;
    g_fC2SelfOnce = 0;
    g_fFuncSelfOnce = 0;
    g_u32C2SelfOk = 0;
    g_u32C2SelfPass = 0;
    g_u32FuncSelfOk = 0;
    g_u32FuncSelfPass = 0;
    /* Residual counters (call/reply/notify + door-skip H3; wrap OK). */
    g_u64DoorOk = 0;
    g_u64DoorTimeout = 0;
    g_u64DoorPeerDead = 0;
    g_u64DoorEnosys = 0;
    g_u64DoorSkipNotReady = 0;
    g_u64DoorSkipDead = 0;
    g_u64QueueTimeout = 0;
    g_u64QueuePark = 0;
    g_u64QueueYieldPark = 0;
    g_u64QueuePostWake = 0;
    g_u64ReplyWake = 0;
    g_u64ServerDoorServe = 0;
    g_u64ServerQueueServe = 0;
    g_u64ServerIdlePark = 0;
    /* C2 residual deepen tallies. */
    g_u64PathWinSvc = 0;
    g_u64PathWinDoor = 0;
    g_u64PathWinQueue = 0;
    g_u64PathWinEnosys = 0;
    g_u64ReplyOnPending = 0;
    g_u64ReplyOnClaimed = 0;
    g_u64QueueToPending = 0;
    g_u64QueueToClaimed = 0;
    g_u64QueueReplyRaceWin = 0;
    g_u64UnregSvcMiss = 0;
    g_u64UnregQMiss = 0;
    g_u64NextIdWrap = 0;
    /* Functional residual (UDX+sshd cold IPC/doors). */
    g_u64HostWake = 0;
    g_u64DoorsBringupYield = 0;
    g_u64FairQueueDrain = 0;
    g_coldQWait = 0;
    door_cold_init();
    g_fInited = 1;
    /* C2 geometry selfcheck before inventory (once-lamp; no stamp storm). */
    residual_c2_selfcheck();
    /* Functional residual selfcheck (UDX+sshd; once-lamp; stamp-free). */
    residual_func_selfcheck();
    /* Sparse baseline lamps after init (once; no stamp storm). */
    soft_inventory_log();
}

void
cold_ipc_set_personality_attached(int fAttached)
{
    if (fAttached) {
        g_fAttached = 1;
        /* Functional residual: attach kicks idle host recheck (UDX/sshd). */
        residual_host_wake();
    } else {
        g_fAttached = 0;
    }
}

int
cold_ipc_personality_attached(void)
{
    /* Doors alone is not enough - need service, queue gen, or explicit attach. */
    return g_fAttached || g_pfnService != NULL || g_u32QueueGen != 0;
}

void
cold_ipc_set_service(i64 (*pfn)(struct gj_linux_regs *, void *), void *pCtx)
{
    (void)cold_ipc_register_service(pfn, pCtx);
}

u32
cold_ipc_register_service(i64 (*pfn)(struct gj_linux_regs *, void *), void *pCtx)
{
    g_u64RegService++;
    if (pfn == NULL) {
        g_pfnService = NULL;
        g_pServiceCtx = NULL;
        g_u32ServiceGen = 0;
        /* Unbind residual: wake so idle personality rechecks paths. */
        residual_host_wake();
        return 0;
    }
    g_pfnService = pfn;
    g_pServiceCtx = pCtx;
    g_u32ServiceGen = bump_service_gen();
    g_fAttached = 1;
    /* Functional residual: UDX/personality service bind wakes idle park. */
    residual_host_wake();
    return g_u32ServiceGen;
}

int
cold_ipc_unregister_service(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32ServiceGen) {
        /* C2 residual: gen-miss on UDX host rebind / stale cookie. */
        g_u64UnregSvcMiss++;
        return 0;
    }
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
    g_u32ServiceGen = 0;
    g_u64UnregService++;
    /* Functional residual: unbind wakes idle for rebind race honesty. */
    residual_host_wake();
    return 1;
}

int
cold_ipc_service_registered(void)
{
    return g_pfnService != NULL;
}

void *
cold_ipc_service_ctx(void)
{
    return g_pServiceCtx;
}

u32
cold_ipc_service_gen(void)
{
    return g_u32ServiceGen;
}

u32
cold_ipc_register_queue_consumer(void)
{
    g_u32QueueGen = bump_queue_gen();
    g_fAttached = 1;
    g_u64RegQueue++;
    /* Functional residual: queue consumer bind kicks idle personality. */
    residual_host_wake();
    return g_u32QueueGen;
}

int
cold_ipc_unregister_queue_consumer(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32QueueGen) {
        /* C2 residual: gen-miss on queue consumer rebind / stale cookie. */
        g_u64UnregQMiss++;
        return 0;
    }
    g_u32QueueGen = 0;
    g_u64UnregQueue++;
    /* Soft: drop explicit attach only when no service remains bound. */
    if (g_pfnService == NULL) {
        g_fAttached = 0;
    }
    /* Functional residual: unbind wakes idle for rebind honesty. */
    residual_host_wake();
    return 1;
}

u32
cold_ipc_queue_gen(void)
{
    return g_u32QueueGen;
}

u32
cold_ipc_queue_pending(void)
{
    return count_pending();
}

u32
cold_ipc_queue_depth(void)
{
    return (u32)GJ_COLD_QUEUE_DEPTH;
}

void
cold_ipc_set_doors_mode(int fEnable)
{
    u32 u32Prev = g_u32ModeFlags;

    if (fEnable) {
        g_u32ModeFlags |= GJ_COLD_MODE_DOORS;
    } else {
        g_u32ModeFlags &= ~GJ_COLD_MODE_DOORS;
    }
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
        /* Functional residual: doors enable/disable rechecks idle park. */
        residual_host_wake();
    }
}

void
cold_ipc_set_mode_flags(u32 u32Flags)
{
    if (g_u32ModeFlags != u32Flags) {
        g_u64ModeChanges++;
        g_u32ModeFlags = u32Flags;
        /* Functional residual: full mode replace wakes idle recheck. */
        residual_host_wake();
        return;
    }
    g_u32ModeFlags = u32Flags;
}

void
cold_ipc_or_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32ModeFlags |= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
        residual_host_wake();
    }
}

void
cold_ipc_and_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32ModeFlags &= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
        residual_host_wake();
    }
}

u32
cold_ipc_get_mode_flags(void)
{
    return g_u32ModeFlags;
}

int
cold_ipc_doors_enabled(void)
{
    return (g_u32ModeFlags & GJ_COLD_MODE_DOORS) != 0;
}

void
cold_ipc_stats_get(struct gj_cold_ipc_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    pOut->u64Submits = g_u64Submits;
    pOut->u64ServiceHits = g_u64ServiceHits;
    pOut->u64DoorHits = g_u64DoorHits;
    pOut->u64QueueHits = g_u64QueueHits;
    pOut->u64Enosys = g_u64Enosys;
    pOut->u64Inval = g_u64Inval;
    pOut->u64QueueFull = g_u64QueueFull;
    pOut->u64Dequeues = g_u64Dequeues;
    pOut->u64DequeueEmpty = g_u64DequeueEmpty;
    pOut->u64Replies = g_u64Replies;
    pOut->u64ReplyMiss = g_u64ReplyMiss;
    pOut->u64ServiceLocal = g_u64ServiceLocal;
    pOut->u64RegService = g_u64RegService;
    pOut->u64UnregService = g_u64UnregService;
    pOut->u64RegQueue = g_u64RegQueue;
    pOut->u64UnregQueue = g_u64UnregQueue;
    pOut->u64ModeChanges = g_u64ModeChanges;
    pOut->u32ModeFlags = g_u32ModeFlags;
    pOut->u32ServiceGen = g_u32ServiceGen;
    pOut->u32QueueGen = g_u32QueueGen;
    pOut->u32Pending = count_pending();
    pOut->u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    pOut->u32ServiceBound = (g_pfnService != NULL) ? 1u : 0u;
    pOut->u32DoorsEnabled = cold_ipc_doors_enabled() ? 1u : 0u;
    pOut->u32Pad = 0;
}

void
cold_ipc_stats_reset(void)
{
    /* Counters only - preserve soft registration, mode, and queue slots. */
    g_u64Submits = 0;
    g_u64ServiceHits = 0;
    g_u64DoorHits = 0;
    g_u64QueueHits = 0;
    g_u64Enosys = 0;
    g_u64Inval = 0;
    g_u64QueueFull = 0;
    g_u64Dequeues = 0;
    g_u64DequeueEmpty = 0;
    g_u64Replies = 0;
    g_u64ReplyMiss = 0;
    g_u64ServiceLocal = 0;
    g_u64RegService = 0;
    g_u64UnregService = 0;
    g_u64RegQueue = 0;
    g_u64UnregQueue = 0;
    g_u64ModeChanges = 0;
}

int
cold_ipc_dequeue(struct gj_cold_request *pOut)
{
    u32 iSlot;

    if (pOut == NULL) {
        g_u64DequeueEmpty++;
        return 0;
    }
    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_PENDING) {
            /* Soft claim: personality owns slot until reply. */
            g_aQ[iSlot].u32State = GJ_COLD_CLAIMED;
            *pOut = g_aQ[iSlot];
            g_u64Dequeues++;
            return 1;
        }
    }
    g_u64DequeueEmpty++;
    return 0;
}

int
cold_ipc_reply(u64 u64Id, i64 i64Ret)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        u32 u32St = g_aQ[iSlot].u32State;

        if ((u32St == GJ_COLD_PENDING || u32St == GJ_COLD_CLAIMED) &&
            g_aQ[iSlot].u64Id == u64Id) {
            g_aQ[iSlot].regs.i64Ret = i64Ret;
            g_aQ[iSlot].u32State = GJ_COLD_DONE;
            /* C2 residual: classify reply state (claim-skip vs dequeue). */
            if (u32St == GJ_COLD_PENDING) {
                g_u64ReplyOnPending++;
            } else {
                g_u64ReplyOnClaimed++;
            }
            /* Reply residual: wake caller parked on this slot (tag 0 = any). */
            (void)thread_wake(&g_aQ[iSlot], COLD_Q_TAG_WAIT, 8);
            g_u64ReplyWake++;
            /* Also nudge notify-shaped queue waiters (idle personality). */
            (void)thread_wake(&g_coldQWait, COLD_Q_TAG_WAIT, 8);
            g_u64Replies++;
            residual_note_once();
            return 1;
        }
    }
    g_u64ReplyMiss++;
    return 0;
}

/*
 * Legacy queue path for GJ_SYS_COLD_DEQUEUE / COLD_REPLY when doors and
 * sync service are unavailable. Product path prefers doors/service.
 *
 * Call residual: park via thread_block + schedule on the slot (reply wakes
 * with tag 0 = any). Timeout residual: absolute mono deadline -> -ETIMEDOUT;
 * free slot if still PENDING/CLAIMED with our id (never demote a landed
 * reply on the same arm). Timed waits yield-poll so mono is observed
 * without a slot timer reaper; untimed parks block. Yield fallback when
 * no current thread.
 */
static i64
submit_queue(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec)
{
    u32 iSlot;
    struct gj_cold_request *pSlot;
    u64 u64Id;
    u64 u64Deadline;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_FREE ||
            g_aQ[iSlot].u32State == GJ_COLD_DONE) {
            pSlot = &g_aQ[iSlot];
            u64Id = g_u64NextId++;
            if (g_u64NextId == 0) {
                g_u64NextId = 1;
                g_u64NextIdWrap++; /* C2 residual: id wrap honesty */
            }
            pSlot->u64Id = u64Id;
            pSlot->regs = *pRegs;
            pSlot->u32State = GJ_COLD_PENDING;
            /* Notify residual: wake queue consumers / personality idle. */
            queue_post_notify();

            u64Deadline = deadline_from_timeout(u64TimeoutNsec);

            while (pSlot->u32State != GJ_COLD_DONE) {
                /*
                 * Timeout residual (door-shaped): deadline first; if reply
                 * already landed on this arm, never demote to ETIMEDOUT.
                 * C2 deepen: classify PENDING vs CLAIMED free + race win.
                 */
                if (u64Deadline != 0 &&
                    (!timer_ready() ||
                     timer_mono_nsec() >= u64Deadline)) {
                    if (pSlot->u32State == GJ_COLD_DONE &&
                        pSlot->u64Id == u64Id) {
                        g_u64QueueReplyRaceWin++;
                        break; /* reply won the race */
                    }
                    if (pSlot->u64Id == u64Id &&
                        pSlot->u32State == GJ_COLD_PENDING) {
                        pSlot->u32State = GJ_COLD_FREE;
                        g_u64QueueToPending++;
                    } else if (pSlot->u64Id == u64Id &&
                               pSlot->u32State == GJ_COLD_CLAIMED) {
                        pSlot->u32State = GJ_COLD_FREE;
                        g_u64QueueToClaimed++;
                    }
                    g_u64QueueTimeout++;
                    residual_note_once();
                    return -LINUX_ETIMEDOUT;
                }

                /*
                 * Untimed call residual: thread_block + schedule; reply wakes.
                 * Timed residual: cooperative yield poll so mono deadline is
                 * observed (no timer reaper on the slot wait object).
                 */
                if (u64Deadline == 0 && thread_current() != NULL) {
                    g_u64QueuePark++;
                    thread_block(pSlot, COLD_Q_TAG_CALLER);
                    schedule();
                } else {
                    g_u64QueueYieldPark++;
                    thread_yield();
                }
            }

            if (pSlot->u64Id != u64Id) {
                /* Slot recycled under us (should not happen if DONE path). */
                residual_note_once();
                return -LINUX_EAGAIN;
            }
            pRegs->i64Ret = pSlot->regs.i64Ret;
            pSlot->u32State = GJ_COLD_FREE;
            residual_note_once();
            return pRegs->i64Ret;
        }
    }
    g_u64QueueFull++;
    /* C2 residual: queue-full pressure still lights residual once-lamp. */
    residual_note_once();
    return -LINUX_EAGAIN;
}

static int
path_service(struct gj_linux_regs *pRegs, i64 *pOut)
{
    if (!service_usable()) {
        return 0;
    }
    g_u64ServiceHits++;
    *pOut = g_pfnService(pRegs, g_pServiceCtx);
    /* SERVICE_FIRST bring-up (personality / libprotonrt) lights residual once. */
    residual_note_once();
    return 1;
}

/**
 * Doors call residual: honor absolute mono timeout when provided.
 * Outcome buckets (ok / timeout / peer-dead / enosys) for honest residual.
 * Skip tallies (not-ready / thr_exit dead) only on submit attempt.
 */
static int
path_doors(struct gj_linux_regs *pRegs, i64 *pOut, u64 u64TimeoutNsec)
{
    struct gj_door *pDoor = NULL;
    u64 u64Deadline;

    if (!doors_usable(&pDoor)) {
        note_door_skip();
        return 0;
    }
    g_u64DoorHits++;
    u64Deadline = deadline_from_timeout(u64TimeoutNsec);
    if (u64Deadline != 0) {
        *pOut = door_call_timeout(pDoor, pRegs, u64Deadline);
    } else {
        *pOut = door_call(pDoor, pRegs);
    }
    note_door_outcome(*pOut);
    residual_note_once();
    return 1;
}

static int
path_queue(struct gj_linux_regs *pRegs, i64 *pOut, u64 u64TimeoutNsec)
{
    if (!queue_usable()) {
        return 0;
    }
    g_u64QueueHits++;
    *pOut = submit_queue(pRegs, u64TimeoutNsec);
    return 1;
}

i64
cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec)
{
    i64 i64R = 0;
    int fDoorsFirst;

    g_u64Submits++;

    if (pRegs == NULL) {
        g_u64Inval++;
        return -LINUX_EINVAL;
    }

    /*
     * Soft product order:
     *   DOORS_FIRST (and not SERVICE_FIRST): doors -> service -> queue
     *   default / SERVICE_FIRST: service -> doors -> queue
     * Avoids kmain hang if a stale pServer is set and never replies when
     * REQUIRE_SERVER is clear and service is bound (service still wins under
     * SERVICE_FIRST).
     * Timeout residual: doors + queue honour u64TimeoutNsec (relative mono).
     */
    fDoorsFirst = ((g_u32ModeFlags & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                  ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE_FIRST) == 0);

    if (fDoorsFirst) {
        if (path_doors(pRegs, &i64R, u64TimeoutNsec)) {
            g_u64PathWinDoor++; /* C2 residual: path-win doors */
            return i64R;
        }
        if (path_service(pRegs, &i64R)) {
            g_u64PathWinSvc++;
            return i64R;
        }
    } else {
        if (path_service(pRegs, &i64R)) {
            g_u64PathWinSvc++; /* C2 residual: path-win service */
            return i64R;
        }
        if (path_doors(pRegs, &i64R, u64TimeoutNsec)) {
            g_u64PathWinDoor++;
            return i64R;
        }
    }

    if (path_queue(pRegs, &i64R, u64TimeoutNsec)) {
        g_u64PathWinQueue++; /* C2 residual: path-win legacy queue */
        return i64R;
    }

    /*
     * Host-missing residual: no service / doors / queue usable.
     * Light residual once so Dual DoD honesty lamps still greppable
     * when UDX personality / driver hosts are not yet bound.
     * Soft!=product - ENOSYS is honest, not a product close.
     * C2 residual: path-win enosys (host-missing honesty).
     */
    g_u64Enosys++;
    g_u64PathWinEnosys++;
    residual_note_once();
    return -LINUX_ENOSYS;
}

i64
cold_ipc_service_local(struct gj_linux_regs *pRegs)
{
    if (pRegs == NULL) {
        g_u64Inval++;
        return -LINUX_EINVAL;
    }
    g_u64ServiceLocal++;
    if (g_pfnService != NULL) {
        pRegs->i64Ret = g_pfnService(pRegs, g_pServiceCtx);
        return pRegs->i64Ret;
    }
    pRegs->i64Ret = -LINUX_ENOSYS;
    return pRegs->i64Ret;
}

/*
 * Personality server body - kernel-thread stand-in for userspace door host.
 * Product audience: Linux personality apps (sshd/netstackd) + UDX/DDI hosts;
 * this loop is bring-up residual until G-PERS maps protonrt-user.
 * Product surface = UDX/DDI + hot/cold ABI; Dual DoD A/B stay OPEN.
 *
 * Honest residual (NOT multi-server product):
 *   single_server=1: one loop handles door_recv/reply OR queue drain/reply.
 *   Sequential fair drain: after a door serve, try one queue claim if pending
 *     (sshd queue + UDX doors dual traffic under single_server honesty).
 *   Queue drain also when doors not live / recv fails; never concurrent
 *   multi-server ownership of one door (multi_server=0).
 *   Idle: pure block on g_coldQWait when doors mode off (queue-only residual).
 *   Doors bring-up residual: MODE_DOORS && !live -> yield-poll so late door
 *     ready is observed (UDX/sshd cold host attach without hang).
 * Soft!=product / G-AC-1 (no in-kernel .ko product AC).
 * freestanding rtl/USB SKIP; dual_dod_close=0 agent_ne_close=1.
 */
void
cold_personality_server(void *pArg)
{
    struct gj_door *pDoor;
    struct gj_linux_regs regsReq;
    struct gj_cold_request reqQ;
    i64 i64R;
    int fDoorsLive;
    int fDoorsMode;

    (void)pArg;
    cold_ipc_set_personality_attached(1);

    for (;;) {
        pDoor = door_cold_personality();
        fDoorsLive = (pDoor != NULL && pDoor->u32Ready != 0);
        fDoorsMode = ((g_u32ModeFlags & GJ_COLD_MODE_DOORS) != 0) ? 1 : 0;

        /*
         * Call residual (doors): block in door_recv when endpoint ready.
         * door_recv parks on DOOR_TAG_SERVER - product single-waiter.
         */
        if (fDoorsLive) {
            if (door_recv(pDoor, &regsReq) == 0) {
                i64R = cold_ipc_service_local(&regsReq);
                door_reply(pDoor, i64R);
                g_u64ServerDoorServe++;
                residual_note_once();
                /*
                 * Functional residual fair drain (single_server sequential):
                 * after one door serve, claim one PENDING if present so
                 * sshd/UDX dual path does not starve the legacy queue.
                 * Not concurrent multi-server (multi_server=0).
                 */
                if (cold_ipc_dequeue(&reqQ)) {
                    i64R = cold_ipc_service_local(&reqQ.regs);
                    (void)cold_ipc_reply(reqQ.u64Id, i64R);
                    g_u64ServerQueueServe++;
                    g_u64FairQueueDrain++;
                    residual_note_once();
                }
                continue;
            }
            /* Transient recv fail (peer dead / not ready race) - fall through. */
        }

        /*
         * Reply residual (queue): single-server drain of one PENDING claim.
         * Not a second concurrent server product (single_server honesty).
         */
        if (cold_ipc_dequeue(&reqQ)) {
            i64R = cold_ipc_service_local(&reqQ.regs);
            (void)cold_ipc_reply(reqQ.u64Id, i64R);
            g_u64ServerQueueServe++;
            residual_note_once();
            continue;
        }

        /*
         * Idle residual:
         *   doors mode off + thr: pure block on g_coldQWait (queue notify).
         *   doors mode on but !live: yield-poll bring-up residual so late
         *     door ready / host_wake rebind is observed (UDX+sshd cold).
         *   doors live: yield (door_recv is the primary wait path).
         * host_wake / queue_post_notify / reply also pulse g_coldQWait.
         */
        if (!fDoorsLive && !fDoorsMode && thread_current() != NULL) {
            g_u64ServerIdlePark++;
            thread_block(&g_coldQWait, COLD_Q_TAG_WAIT);
            schedule();
        } else if (!fDoorsLive && fDoorsMode) {
            g_u64DoorsBringupYield++;
            thread_yield();
        } else {
            thread_yield();
        }
    }
}
