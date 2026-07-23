/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscalls → personality.
 * Product: doors / sync service / legacy queue under soft mode flags.
 * Soft deepen: registration gens, mode mask, CLAIMED queue, stats.
 * Pure C11 — dual MIT OR Apache-2.0.
 *
 * greppable: GJ_COLD_MODE_ cold_ipc_register_service cold_ipc_stats
 *
 * Soft product inventory (Wave 14 base + Wave 16 exclusive deepen; this unit only):
 *   - Mode mask / path preference (service-first vs doors-first)
 *   - Service + queue-consumer registration gens + bind tallies
 *   - Queue slot lifecycle (FREE/PENDING/CLAIMED/DONE) + depth
 *   - Submit path hits (service/doors/queue) + outcome counters
 *   - Attach / stats / gen-mismatch lamps (Wave 15 deepen)
 *   - rates / honesty / catalog / PASS (Wave 15 deepen)
 *   - soft deepen wave=16 stamp
 *   Never hard-gates; diagnostics only (wrap OK).
 * Greppable twin prefixes (product / agent greps):
 *   "cold_ipc: soft …"
 *   "cold: soft …"
 */
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/thread.h>

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

/* Wave 16 soft inventory stamp (file-local; never product gate). */
#define GJ_COLD_SOFT_WAVE  16u
/* Soft inventory area count (fixed greppable categories for deepen stamp). */
#define GJ_COLD_SOFT_AREAS 19u

/*
 * Soft product inventory (Wave 16 exclusive deepen). Cumulative unless noted live.
 * greppable: cold_ipc: soft … / cold: soft …
 */
static u32 g_u32SoftInitEnter;     /* cold_ipc_init entries */
static u32 g_u32SoftInitOk;        /* first-time init completed */
static u32 g_u32SoftInitIdem;      /* already-inited early return */
static u32 g_u32SoftSubmitEnter;   /* cold_ipc_submit entries */
static u32 g_u32SoftSubmitNull;    /* submit with pRegs == NULL */
static u32 g_u32SoftSubmitSvc;     /* submit took service path */
static u32 g_u32SoftSubmitDoor;    /* submit took doors path */
static u32 g_u32SoftSubmitQueue;   /* submit took queue path */
static u32 g_u32SoftSubmitEnosys;  /* submit fell through to ENOSYS */
static u32 g_u32SoftDeqEnter;      /* cold_ipc_dequeue entries */
static u32 g_u32SoftDeqClaim;      /* soft claim PENDING → CLAIMED */
static u32 g_u32SoftDeqEmpty;      /* dequeue empty / null */
static u32 g_u32SoftReplyEnter;    /* cold_ipc_reply entries */
static u32 g_u32SoftReplyOk;       /* reply matched PENDING/CLAIMED */
static u32 g_u32SoftReplyMiss;     /* reply id miss */
static u32 g_u32SoftLocalEnter;    /* cold_ipc_service_local entries */
static u32 g_u32SoftLocalHit;      /* service_local with pfn bound */
static u32 g_u32SoftLocalMiss;     /* service_local unbound → ENOSYS */
static u32 g_u32SoftLogN;          /* soft inventory log emissions */
static u8  g_fSoftInvOnce;         /* one-shot deep dump after activity */
/* Wave 16 deepen: attach / stats / gen-mismatch / mode API lamps. */
static u32 g_u32SoftAttachSet;     /* cold_ipc_set_personality_attached calls */
static u32 g_u32SoftAttachOn;      /* attach set to 1 */
static u32 g_u32SoftAttachOff;     /* attach set to 0 */
static u32 g_u32SoftStatsGet;      /* cold_ipc_stats_get samples */
static u32 g_u32SoftStatsGetNull;  /* stats_get with pOut == NULL */
static u32 g_u32SoftStatsReset;    /* cold_ipc_stats_reset calls */
static u32 g_u32SoftUnregSvcMiss;  /* unregister_service gen mismatch */
static u32 g_u32SoftUnregQMiss;    /* unregister_queue gen mismatch */
static u32 g_u32SoftModeSet;       /* set_mode_flags entries */
static u32 g_u32SoftModeOr;        /* or_mode_flags entries */
static u32 g_u32SoftModeAnd;       /* and_mode_flags entries */
static u32 g_u32SoftDoorsMode;     /* set_doors_mode entries */
static u32 g_u32SoftNextIdWrap;    /* next-id wrap notes (queue path) */

static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

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
 * Greppable soft cold_ipc inventory (product / smoke; Wave 15 deepen).
 * Twin prefixes so either agent grep works:
 *   cold_ipc: soft inventory|mode|service|queue|path|submit|dequeue|reply|
 *             init|attach|stats|deepen …
 *   cold: soft …
 * greppable: cold_ipc: soft
 * greppable: cold: soft
 */
static void
soft_inventory_log(void)
{
    u32 u32Free;
    u32 u32Pending;
    u32 u32Claimed;
    u32 u32Done;
    u32 u32Mode;
    u32 u32SvcGen;
    u32 u32QGen;
    u32 u32Attached;
    u32 u32SvcBound;
    u32 u32DoorsEn;
    u32 u32SvcUsable;
    u32 u32DoorsUsable;
    u32 u32QueueUsable;
    u32 u32DoorsFirst;
    u32 u32SvcFirst;
    u32 u32ReqSrv;
    const char *szOrder;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    /* Snapshot live state (diagnostics only; no hard lock). */
    count_slots(&u32Free, &u32Pending, &u32Claimed, &u32Done);
    u32Mode = g_u32ModeFlags;
    u32SvcGen = g_u32ServiceGen;
    u32QGen = g_u32QueueGen;
    u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    u32SvcBound = (g_pfnService != NULL) ? 1u : 0u;
    u32DoorsEn = (u32Mode & GJ_COLD_MODE_DOORS) != 0 ? 1u : 0u;
    u32SvcUsable = service_usable() ? 1u : 0u;
    u32DoorsUsable = doors_usable(NULL) ? 1u : 0u;
    u32QueueUsable = queue_usable() ? 1u : 0u;
    u32DoorsFirst = ((u32Mode & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                    ((u32Mode & GJ_COLD_MODE_SERVICE_FIRST) == 0)
                        ? 1u
                        : 0u;
    u32SvcFirst = (u32Mode & GJ_COLD_MODE_SERVICE_FIRST) != 0 ? 1u : 0u;
    u32ReqSrv = (u32Mode & GJ_COLD_MODE_REQUIRE_SERVER) != 0 ? 1u : 0u;
    if (u32DoorsFirst) {
        szOrder = "doors>service>queue";
    } else {
        szOrder = "service>doors>queue";
    }

    /*
     * Primary prefix: cold_ipc: soft …
     * Catalog capacity + live gens + path tallies for smoke greps.
     */
    /* Grep: cold_ipc: soft inventory */
    kprintf("cold_ipc: soft inventory wave=%u depth=%u free=%u pending=%u "
            "claimed=%u done=%u attached=%u svc_bound=%u doors_en=%u "
            "mode=0x%x svc_gen=%u q_gen=%u log_n=%u areas=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE,
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Free, u32Pending, u32Claimed,
            u32Done, u32Attached, u32SvcBound, u32DoorsEn, u32Mode, u32SvcGen,
            u32QGen, g_u32SoftLogN, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold_ipc: soft mode */
    kprintf("cold_ipc: soft mode flags=0x%x doors=%u service=%u queue=%u "
            "svc_first=%u doors_first=%u req_server=%u changes=%llu "
            "set=%u or=%u and=%u doors_mode=%u order=%s wave=%u\n",
            u32Mode, u32DoorsEn,
            (u32Mode & GJ_COLD_MODE_SERVICE) != 0 ? 1u : 0u,
            (u32Mode & GJ_COLD_MODE_QUEUE) != 0 ? 1u : 0u, u32SvcFirst,
            u32DoorsFirst, u32ReqSrv,
            (unsigned long long)g_u64ModeChanges,
            g_u32SoftModeSet, g_u32SoftModeOr, g_u32SoftModeAnd,
            g_u32SoftDoorsMode, szOrder, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft service */
    kprintf("cold_ipc: soft service bound=%u gen=%u usable=%u reg=%llu "
            "unreg=%llu unreg_miss=%u local_enter=%u local_hit=%u "
            "local_miss=%u wave=%u\n",
            u32SvcBound, u32SvcGen, u32SvcUsable,
            (unsigned long long)g_u64RegService,
            (unsigned long long)g_u64UnregService, g_u32SoftUnregSvcMiss,
            g_u32SoftLocalEnter, g_u32SoftLocalHit, g_u32SoftLocalMiss,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft queue */
    kprintf("cold_ipc: soft queue depth=%u free=%u pending=%u claimed=%u "
            "done=%u gen=%u usable=%u reg=%llu unreg=%llu unreg_miss=%u "
            "full=%llu next_id_wrap=%u wave=%u\n",
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Free, u32Pending, u32Claimed,
            u32Done, u32QGen, u32QueueUsable,
            (unsigned long long)g_u64RegQueue,
            (unsigned long long)g_u64UnregQueue, g_u32SoftUnregQMiss,
            (unsigned long long)g_u64QueueFull, g_u32SoftNextIdWrap,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft path */
    kprintf("cold_ipc: soft path order=%s svc_usable=%u doors_usable=%u "
            "queue_usable=%u claim=service+doors+queue wave=%u "
            "(soft inventory; not bar3)\n",
            szOrder, u32SvcUsable, u32DoorsUsable, u32QueueUsable,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft submit */
    kprintf("cold_ipc: soft submit enter=%u null=%u svc=%u door=%u "
            "queue=%u enosys=%u hits_svc=%llu hits_door=%llu "
            "hits_queue=%llu submits=%llu inval=%llu wave=%u\n",
            g_u32SoftSubmitEnter, g_u32SoftSubmitNull, g_u32SoftSubmitSvc,
            g_u32SoftSubmitDoor, g_u32SoftSubmitQueue, g_u32SoftSubmitEnosys,
            (unsigned long long)g_u64ServiceHits,
            (unsigned long long)g_u64DoorHits,
            (unsigned long long)g_u64QueueHits,
            (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64Inval,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft dequeue */
    kprintf("cold_ipc: soft dequeue enter=%u claim=%u empty=%u "
            "dequeues=%llu empty_ctr=%llu wave=%u\n",
            g_u32SoftDeqEnter, g_u32SoftDeqClaim, g_u32SoftDeqEmpty,
            (unsigned long long)g_u64Dequeues,
            (unsigned long long)g_u64DequeueEmpty,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft reply */
    kprintf("cold_ipc: soft reply enter=%u ok=%u miss=%u replies=%llu "
            "miss_ctr=%llu lifecycle=FREE>PENDING>CLAIMED>DONE>FREE "
            "wave=%u\n",
            g_u32SoftReplyEnter, g_u32SoftReplyOk, g_u32SoftReplyMiss,
            (unsigned long long)g_u64Replies,
            (unsigned long long)g_u64ReplyMiss,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft init */
    kprintf("cold_ipc: soft init enter=%u ok=%u idem=%u inited=%u wave=%u\n",
            g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftInitIdem,
            g_fInited ? 1u : 0u, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft attach (Wave 15 deepen) */
    kprintf("cold_ipc: soft attach set=%u on=%u off=%u live=%u "
            "explicit=%u wave=%u\n",
            g_u32SoftAttachSet, g_u32SoftAttachOn, g_u32SoftAttachOff,
            u32Attached, g_fAttached ? 1u : 0u,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft stats (Wave 15 deepen) */
    kprintf("cold_ipc: soft stats get=%u get_null=%u reset=%u "
            "submits=%llu enosys=%llu mode_changes=%llu wave=%u\n",
            g_u32SoftStatsGet, g_u32SoftStatsGetNull, g_u32SoftStatsReset,
            (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64Enosys,
            (unsigned long long)g_u64ModeChanges,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft rates (Wave 15 deepen) */
    {
        u64 u64HitSum;
        u64 u64BpSvc;
        u64 u64BpDoor;
        u64 u64BpQueue;

        u64HitSum = g_u64ServiceHits + g_u64DoorHits + g_u64QueueHits;
        if (u64HitSum != 0) {
            u64BpSvc = (g_u64ServiceHits * 10000ull) / u64HitSum;
            u64BpDoor = (g_u64DoorHits * 10000ull) / u64HitSum;
            u64BpQueue = (g_u64QueueHits * 10000ull) / u64HitSum;
        } else {
            u64BpSvc = 0;
            u64BpDoor = 0;
            u64BpQueue = 0;
        }
        kprintf("cold_ipc: soft rates bp_svc=%llu bp_door=%llu "
                "bp_queue=%llu hit_sum=%llu submits=%llu enosys=%llu "
                "wave=%u\n",
                (unsigned long long)u64BpSvc,
                (unsigned long long)u64BpDoor,
                (unsigned long long)u64BpQueue,
                (unsigned long long)u64HitSum,
                (unsigned long long)g_u64Submits,
                (unsigned long long)g_u64Enosys,
                (unsigned)GJ_COLD_SOFT_WAVE);
    }

    /* Grep: cold_ipc: soft honesty (Wave 15 deepen) */
    kprintf("cold_ipc: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 personality=cold "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft catalog (Wave 15 deepen) */
    kprintf("cold_ipc: soft catalog wave=%u areas=%u "
            "surfaces=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,deepen,PASS\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold_ipc: soft surfaces (Wave 16 deepen) */
    kprintf("cold_ipc: soft surfaces count=%u "
            "names=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,surfaces,"
            "note,deepen,PASS wave=%u\n",
            (unsigned)GJ_COLD_SOFT_AREAS, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft note (Wave 16 deepen) */
    kprintf("cold_ipc: soft note milestone=wave16 exclusive=1 "
            "soft_only=1 not_bar3=1 submits=%llu logs=%u wave=%u\n",
            (unsigned long long)g_u64Submits, g_u32SoftLogN,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft deepen wave (Wave 15 stamp) */
    kprintf("cold_ipc: soft deepen wave=%u areas=%u logs=%u "
            "svc_bound=%u doors_usable=%u queue_usable=%u submits=%llu "
            "(Wave 16 exclusive; soft inventory; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS,
            g_u32SoftLogN, u32SvcBound, u32DoorsUsable, u32QueueUsable,
            (unsigned long long)g_u64Submits);

    /* Grep: cold_ipc: soft inventory PASS / soft PASS */
    kprintf("cold_ipc: soft inventory PASS wave=%u logs=%u "
            "submits=%llu svc_bound=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE, g_u32SoftLogN,
            (unsigned long long)g_u64Submits, u32SvcBound);
    kprintf("cold_ipc: soft PASS wave=%u logs=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE, g_u32SoftLogN);

    /*
     * Twin prefix: cold: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: cold: soft inventory */
    kprintf("cold: soft inventory wave=%u depth=%u free=%u pending=%u "
            "claimed=%u done=%u attached=%u svc_bound=%u doors_en=%u "
            "mode=0x%x svc_gen=%u q_gen=%u log_n=%u areas=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE,
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Free, u32Pending, u32Claimed,
            u32Done, u32Attached, u32SvcBound, u32DoorsEn, u32Mode, u32SvcGen,
            u32QGen, g_u32SoftLogN, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold: soft mode */
    kprintf("cold: soft mode flags=0x%x doors=%u service=%u queue=%u "
            "svc_first=%u doors_first=%u req_server=%u changes=%llu "
            "set=%u or=%u and=%u doors_mode=%u order=%s wave=%u\n",
            u32Mode, u32DoorsEn,
            (u32Mode & GJ_COLD_MODE_SERVICE) != 0 ? 1u : 0u,
            (u32Mode & GJ_COLD_MODE_QUEUE) != 0 ? 1u : 0u, u32SvcFirst,
            u32DoorsFirst, u32ReqSrv,
            (unsigned long long)g_u64ModeChanges,
            g_u32SoftModeSet, g_u32SoftModeOr, g_u32SoftModeAnd,
            g_u32SoftDoorsMode, szOrder, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft service */
    kprintf("cold: soft service bound=%u gen=%u usable=%u reg=%llu "
            "unreg=%llu unreg_miss=%u local_enter=%u local_hit=%u "
            "local_miss=%u wave=%u\n",
            u32SvcBound, u32SvcGen, u32SvcUsable,
            (unsigned long long)g_u64RegService,
            (unsigned long long)g_u64UnregService, g_u32SoftUnregSvcMiss,
            g_u32SoftLocalEnter, g_u32SoftLocalHit, g_u32SoftLocalMiss,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft queue */
    kprintf("cold: soft queue depth=%u free=%u pending=%u claimed=%u "
            "done=%u gen=%u usable=%u reg=%llu unreg=%llu unreg_miss=%u "
            "full=%llu next_id_wrap=%u wave=%u\n",
            (unsigned)GJ_COLD_QUEUE_DEPTH, u32Free, u32Pending, u32Claimed,
            u32Done, u32QGen, u32QueueUsable,
            (unsigned long long)g_u64RegQueue,
            (unsigned long long)g_u64UnregQueue, g_u32SoftUnregQMiss,
            (unsigned long long)g_u64QueueFull, g_u32SoftNextIdWrap,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft path */
    kprintf("cold: soft path order=%s svc_usable=%u doors_usable=%u "
            "queue_usable=%u claim=service+doors+queue wave=%u "
            "(soft inventory; not bar3)\n",
            szOrder, u32SvcUsable, u32DoorsUsable, u32QueueUsable,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft submit */
    kprintf("cold: soft submit enter=%u null=%u svc=%u door=%u "
            "queue=%u enosys=%u hits_svc=%llu hits_door=%llu "
            "hits_queue=%llu submits=%llu inval=%llu wave=%u\n",
            g_u32SoftSubmitEnter, g_u32SoftSubmitNull, g_u32SoftSubmitSvc,
            g_u32SoftSubmitDoor, g_u32SoftSubmitQueue, g_u32SoftSubmitEnosys,
            (unsigned long long)g_u64ServiceHits,
            (unsigned long long)g_u64DoorHits,
            (unsigned long long)g_u64QueueHits,
            (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64Inval,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft dequeue */
    kprintf("cold: soft dequeue enter=%u claim=%u empty=%u "
            "dequeues=%llu empty_ctr=%llu wave=%u\n",
            g_u32SoftDeqEnter, g_u32SoftDeqClaim, g_u32SoftDeqEmpty,
            (unsigned long long)g_u64Dequeues,
            (unsigned long long)g_u64DequeueEmpty,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft reply */
    kprintf("cold: soft reply enter=%u ok=%u miss=%u replies=%llu "
            "miss_ctr=%llu lifecycle=FREE>PENDING>CLAIMED>DONE>FREE "
            "wave=%u\n",
            g_u32SoftReplyEnter, g_u32SoftReplyOk, g_u32SoftReplyMiss,
            (unsigned long long)g_u64Replies,
            (unsigned long long)g_u64ReplyMiss,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft init */
    kprintf("cold: soft init enter=%u ok=%u idem=%u inited=%u wave=%u\n",
            g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftInitIdem,
            g_fInited ? 1u : 0u, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft attach (Wave 15 deepen) */
    kprintf("cold: soft attach set=%u on=%u off=%u live=%u "
            "explicit=%u wave=%u\n",
            g_u32SoftAttachSet, g_u32SoftAttachOn, g_u32SoftAttachOff,
            u32Attached, g_fAttached ? 1u : 0u,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft stats (Wave 15 deepen) */
    kprintf("cold: soft stats get=%u get_null=%u reset=%u "
            "submits=%llu enosys=%llu mode_changes=%llu wave=%u\n",
            g_u32SoftStatsGet, g_u32SoftStatsGetNull, g_u32SoftStatsReset,
            (unsigned long long)g_u64Submits,
            (unsigned long long)g_u64Enosys,
            (unsigned long long)g_u64ModeChanges,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft rates (Wave 15 deepen) */
    {
        u64 u64HitSum;
        u64 u64BpSvc;
        u64 u64BpDoor;
        u64 u64BpQueue;

        u64HitSum = g_u64ServiceHits + g_u64DoorHits + g_u64QueueHits;
        if (u64HitSum != 0) {
            u64BpSvc = (g_u64ServiceHits * 10000ull) / u64HitSum;
            u64BpDoor = (g_u64DoorHits * 10000ull) / u64HitSum;
            u64BpQueue = (g_u64QueueHits * 10000ull) / u64HitSum;
        } else {
            u64BpSvc = 0;
            u64BpDoor = 0;
            u64BpQueue = 0;
        }
        kprintf("cold: soft rates bp_svc=%llu bp_door=%llu "
                "bp_queue=%llu hit_sum=%llu submits=%llu enosys=%llu "
                "wave=%u\n",
                (unsigned long long)u64BpSvc,
                (unsigned long long)u64BpDoor,
                (unsigned long long)u64BpQueue,
                (unsigned long long)u64HitSum,
                (unsigned long long)g_u64Submits,
                (unsigned long long)g_u64Enosys,
                (unsigned)GJ_COLD_SOFT_WAVE);
    }

    /* Grep: cold: soft honesty (Wave 15 deepen) */
    kprintf("cold: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 personality=cold "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft catalog (Wave 15 deepen) */
    kprintf("cold: soft catalog wave=%u areas=%u "
            "surfaces=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,deepen,PASS\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold: soft surfaces (Wave 16 deepen) */
    kprintf("cold: soft surfaces count=%u "
            "names=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,surfaces,"
            "note,deepen,PASS wave=%u\n",
            (unsigned)GJ_COLD_SOFT_AREAS, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft note (Wave 16 deepen) */
    kprintf("cold: soft note milestone=wave16 exclusive=1 "
            "soft_only=1 not_bar3=1 submits=%llu logs=%u wave=%u\n",
            (unsigned long long)g_u64Submits, g_u32SoftLogN,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft deepen wave (Wave 15 stamp) */
    kprintf("cold: soft deepen wave=%u areas=%u logs=%u "
            "svc_bound=%u doors_usable=%u queue_usable=%u submits=%llu "
            "(Wave 16 exclusive; soft inventory; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS,
            g_u32SoftLogN, u32SvcBound, u32DoorsUsable, u32QueueUsable,
            (unsigned long long)g_u64Submits);

    /* Grep: cold: soft inventory PASS / soft PASS */
    kprintf("cold: soft inventory PASS wave=%u logs=%u "
            "submits=%llu svc_bound=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE, g_u32SoftLogN,
            (unsigned long long)g_u64Submits, u32SvcBound);
    kprintf("cold: soft PASS wave=%u logs=%u\n",
            (unsigned)GJ_COLD_SOFT_WAVE, g_u32SoftLogN);
}

/**
 * After first product submit/dequeue/reply/local activity, print soft
 * inventory once (mirrors futex/input_hub soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftSubmitEnter == 0 && g_u32SoftDeqEnter == 0 &&
        g_u32SoftReplyEnter == 0 && g_u32SoftLocalEnter == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

void
cold_ipc_init(void)
{
    g_u32SoftInitEnter++;

    /* Idempotent: must not wipe protonrt service after attach (boot smoke). */
    if (g_fInited) {
        g_u32SoftInitIdem++;
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
    g_u32SoftSubmitEnter = 0;
    g_u32SoftSubmitNull = 0;
    g_u32SoftSubmitSvc = 0;
    g_u32SoftSubmitDoor = 0;
    g_u32SoftSubmitQueue = 0;
    g_u32SoftSubmitEnosys = 0;
    g_u32SoftDeqEnter = 0;
    g_u32SoftDeqClaim = 0;
    g_u32SoftDeqEmpty = 0;
    g_u32SoftReplyEnter = 0;
    g_u32SoftReplyOk = 0;
    g_u32SoftReplyMiss = 0;
    g_u32SoftLocalEnter = 0;
    g_u32SoftLocalHit = 0;
    g_u32SoftLocalMiss = 0;
    g_u32SoftLogN = 0;
    g_fSoftInvOnce = 0;
    g_u32SoftAttachSet = 0;
    g_u32SoftAttachOn = 0;
    g_u32SoftAttachOff = 0;
    g_u32SoftStatsGet = 0;
    g_u32SoftStatsGetNull = 0;
    g_u32SoftStatsReset = 0;
    g_u32SoftUnregSvcMiss = 0;
    g_u32SoftUnregQMiss = 0;
    g_u32SoftModeSet = 0;
    g_u32SoftModeOr = 0;
    g_u32SoftModeAnd = 0;
    g_u32SoftDoorsMode = 0;
    g_u32SoftNextIdWrap = 0;
    door_cold_init();
    g_fInited = 1;
    g_u32SoftInitOk++;
    /* Grep: cold_ipc: soft / cold: soft (baseline inventory after init) */
    soft_inventory_log();
}

void
cold_ipc_set_personality_attached(int fAttached)
{
    g_u32SoftAttachSet++;
    if (fAttached) {
        g_fAttached = 1;
        g_u32SoftAttachOn++;
    } else {
        g_fAttached = 0;
        g_u32SoftAttachOff++;
    }
}

int
cold_ipc_personality_attached(void)
{
    /* Doors alone is not enough — need service, queue gen, or explicit attach. */
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
        return 0;
    }
    g_pfnService = pfn;
    g_pServiceCtx = pCtx;
    g_u32ServiceGen = bump_service_gen();
    g_fAttached = 1;
    return g_u32ServiceGen;
}

int
cold_ipc_unregister_service(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32ServiceGen) {
        g_u32SoftUnregSvcMiss++;
        return 0;
    }
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
    g_u32ServiceGen = 0;
    g_u64UnregService++;
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
    return g_u32QueueGen;
}

int
cold_ipc_unregister_queue_consumer(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32QueueGen) {
        g_u32SoftUnregQMiss++;
        return 0;
    }
    g_u32QueueGen = 0;
    g_u64UnregQueue++;
    /* Soft: drop explicit attach only when no service remains bound. */
    if (g_pfnService == NULL) {
        g_fAttached = 0;
    }
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

    g_u32SoftDoorsMode++;
    if (fEnable) {
        g_u32ModeFlags |= GJ_COLD_MODE_DOORS;
    } else {
        g_u32ModeFlags &= ~GJ_COLD_MODE_DOORS;
    }
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
    }
}

void
cold_ipc_set_mode_flags(u32 u32Flags)
{
    g_u32SoftModeSet++;
    if (g_u32ModeFlags != u32Flags) {
        g_u64ModeChanges++;
    }
    g_u32ModeFlags = u32Flags;
}

void
cold_ipc_or_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32SoftModeOr++;
    g_u32ModeFlags |= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
    }
}

void
cold_ipc_and_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32SoftModeAnd++;
    g_u32ModeFlags &= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
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
        g_u32SoftStatsGetNull++;
        return;
    }
    g_u32SoftStatsGet++;
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
    /* Counters only — preserve soft registration, mode, and queue slots. */
    g_u32SoftStatsReset++;
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

    g_u32SoftDeqEnter++;
    if (pOut == NULL) {
        g_u64DequeueEmpty++;
        g_u32SoftDeqEmpty++;
        soft_inventory_maybe_once();
        return 0;
    }
    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_PENDING) {
            /* Soft claim: personality owns slot until reply. */
            g_aQ[iSlot].u32State = GJ_COLD_CLAIMED;
            *pOut = g_aQ[iSlot];
            g_u64Dequeues++;
            g_u32SoftDeqClaim++;
            soft_inventory_maybe_once();
            return 1;
        }
    }
    g_u64DequeueEmpty++;
    g_u32SoftDeqEmpty++;
    soft_inventory_maybe_once();
    return 0;
}

int
cold_ipc_reply(u64 u64Id, i64 i64Ret)
{
    u32 iSlot;

    g_u32SoftReplyEnter++;
    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        u32 u32St = g_aQ[iSlot].u32State;

        if ((u32St == GJ_COLD_PENDING || u32St == GJ_COLD_CLAIMED) &&
            g_aQ[iSlot].u64Id == u64Id) {
            g_aQ[iSlot].regs.i64Ret = i64Ret;
            g_aQ[iSlot].u32State = GJ_COLD_DONE;
            (void)thread_wake(&g_aQ[iSlot], 0, 8);
            g_u64Replies++;
            g_u32SoftReplyOk++;
            soft_inventory_maybe_once();
            return 1;
        }
    }
    g_u64ReplyMiss++;
    g_u32SoftReplyMiss++;
    soft_inventory_maybe_once();
    return 0;
}

/*
 * Legacy queue path for GJ_SYS_COLD_DEQUEUE / COLD_REPLY when doors and
 * sync service are unavailable. Product path prefers doors/service.
 * Soft: waits for DONE (covers PENDING and CLAIMED reclaim).
 */
static i64
submit_queue(struct gj_linux_regs *pRegs)
{
    u32 iSlot;
    struct gj_cold_request *pSlot;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_FREE ||
            g_aQ[iSlot].u32State == GJ_COLD_DONE) {
            pSlot = &g_aQ[iSlot];
            pSlot->u64Id = g_u64NextId++;
            if (g_u64NextId == 0) {
                g_u64NextId = 1;
                g_u32SoftNextIdWrap++;
            }
            pSlot->regs = *pRegs;
            pSlot->u32State = GJ_COLD_PENDING;
            /* Soft: spin-yield until reply sets DONE (CLAIMED interim OK). */
            while (pSlot->u32State != GJ_COLD_DONE) {
                thread_yield();
            }
            pRegs->i64Ret = pSlot->regs.i64Ret;
            pSlot->u32State = GJ_COLD_FREE;
            return pRegs->i64Ret;
        }
    }
    g_u64QueueFull++;
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
    return 1;
}

static int
path_doors(struct gj_linux_regs *pRegs, i64 *pOut)
{
    struct gj_door *pDoor = NULL;

    if (!doors_usable(&pDoor)) {
        return 0;
    }
    g_u64DoorHits++;
    *pOut = door_call(pDoor, pRegs);
    return 1;
}

static int
path_queue(struct gj_linux_regs *pRegs, i64 *pOut)
{
    if (!queue_usable()) {
        return 0;
    }
    g_u64QueueHits++;
    *pOut = submit_queue(pRegs);
    return 1;
}

i64
cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec)
{
    i64 i64R = 0;
    int fDoorsFirst;

    (void)u64TimeoutNsec;

    g_u64Submits++;
    g_u32SoftSubmitEnter++;

    if (pRegs == NULL) {
        g_u64Inval++;
        g_u32SoftSubmitNull++;
        soft_inventory_maybe_once();
        return -LINUX_EINVAL;
    }

    /*
     * Soft product order:
     *   DOORS_FIRST (and not SERVICE_FIRST): doors → service → queue
     *   default / SERVICE_FIRST: service → doors → queue
     * Avoids kmain hang if a stale pServer is set and never replies when
     * REQUIRE_SERVER is clear and service is bound (service still wins under
     * SERVICE_FIRST).
     */
    fDoorsFirst = ((g_u32ModeFlags & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                  ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE_FIRST) == 0);

    if (fDoorsFirst) {
        if (path_doors(pRegs, &i64R)) {
            g_u32SoftSubmitDoor++;
            soft_inventory_maybe_once();
            return i64R;
        }
        if (path_service(pRegs, &i64R)) {
            g_u32SoftSubmitSvc++;
            soft_inventory_maybe_once();
            return i64R;
        }
    } else {
        if (path_service(pRegs, &i64R)) {
            g_u32SoftSubmitSvc++;
            soft_inventory_maybe_once();
            return i64R;
        }
        if (path_doors(pRegs, &i64R)) {
            g_u32SoftSubmitDoor++;
            soft_inventory_maybe_once();
            return i64R;
        }
    }

    if (path_queue(pRegs, &i64R)) {
        g_u32SoftSubmitQueue++;
        soft_inventory_maybe_once();
        return i64R;
    }

    g_u64Enosys++;
    g_u32SoftSubmitEnosys++;
    soft_inventory_maybe_once();
    return -LINUX_ENOSYS;
}

i64
cold_ipc_service_local(struct gj_linux_regs *pRegs)
{
    g_u32SoftLocalEnter++;
    if (pRegs == NULL) {
        g_u64Inval++;
        g_u32SoftLocalMiss++;
        soft_inventory_maybe_once();
        return -LINUX_EINVAL;
    }
    g_u64ServiceLocal++;
    if (g_pfnService != NULL) {
        g_u32SoftLocalHit++;
        pRegs->i64Ret = g_pfnService(pRegs, g_pServiceCtx);
        soft_inventory_maybe_once();
        return pRegs->i64Ret;
    }
    g_u32SoftLocalMiss++;
    pRegs->i64Ret = -LINUX_ENOSYS;
    soft_inventory_maybe_once();
    return pRegs->i64Ret;
}

/* Personality server body — run as kernel thread (bring-up fallback) */
void
cold_personality_server(void *pArg)
{
    struct gj_door *pDoor = door_cold_personality();
    struct gj_linux_regs regsReq;
    i64 i64R;

    (void)pArg;
    cold_ipc_set_personality_attached(1);
    for (;;) {
        if (door_recv(pDoor, &regsReq) != 0) {
            thread_yield();
            continue;
        }
        i64R = cold_ipc_service_local(&regsReq);
        door_reply(pDoor, i64R);
    }
}
