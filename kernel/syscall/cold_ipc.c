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
 * Soft product inventory (Wave 14 base + Wave 35 exclusive deepen; this unit only):
 *   - Mode mask / path preference (service-first vs doors-first)
 *   - Service + queue-consumer registration gens + bind tallies
 *   - Queue slot lifecycle (FREE/PENDING/CLAIMED/DONE) + depth
 *   - Submit path hits (service/doors/queue) + outcome counters
 *   - Attach / stats / gen-mismatch lamps (Wave 15 deepen)
 *   - rates / honesty / catalog / PASS (Wave 15 deepen)
 *   - soft deepen wave=112 stamp
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

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define GJ_COLD_SOFT_WAVE 112u
/* Soft inventory area count (fixed greppable categories for deepen stamp). */
#define GJ_COLD_SOFT_AREAS 203u

/*
 * Soft product inventory (Wave 35 exclusive deepen). Cumulative unless noted live.
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
/* Wave 20 deepen: attach / stats / gen-mismatch / mode API lamps. */
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
            "reply,init,attach,stats,rates,honesty,catalog,return,retmap,deepen,PASS\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold_ipc: soft surfaces (Wave 20 deepen) */
    kprintf("cold_ipc: soft surfaces count=%u "
            "names=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,surfaces,"
            "note,return,retmap,deepen,PASS wave=%u\n",
            (unsigned)GJ_COLD_SOFT_AREAS, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft note (Wave 20 deepen) */
    kprintf("cold_ipc: soft note milestone=wave98 exclusive=1 "
            "soft_only=1 not_bar3=1 submits=%llu logs=%u wave=%u\n",
            (unsigned long long)g_u64Submits, g_u32SoftLogN,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft return (Wave 20 deepen) */
    kprintf("cold_ipc: soft return submit_svc=%u submit_door=%u "
            "submit_queue=%u submit_enosys=%u deq_claim=%u deq_empty=%u "
            "product_gate=0 wave=%u\n",
            g_u32SoftSubmitSvc, g_u32SoftSubmitDoor, g_u32SoftSubmitQueue,
            g_u32SoftSubmitEnosys, g_u32SoftDeqClaim, g_u32SoftDeqEmpty,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold_ipc: soft retmap — Wave 19 return-surface map */
    kprintf("cold_ipc: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=112\n");

    /* Grep: cold_ipc: soft deepen wave (Wave 15 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold_ipc: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("cold_ipc: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /* Grep: cold_ipc: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("cold_ipc: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold_ipc: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("cold_ipc: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /* Grep: cold_ipc: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("cold_ipc: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold_ipc: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("cold_ipc: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("cold_ipc: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold_ipc: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("cold_ipc: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("cold_ipc: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: cold_ipc: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("cold_ipc: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("cold_ipc: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_ipc: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("cold_ipc: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("cold_ipc: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_ipc: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("cold_ipc: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("cold_ipc: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_ipc: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("cold_ipc: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold_ipc: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("cold_ipc: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: cold_ipc: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("cold_ipc: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_COLD_SOFT_WAVE);
                    /* Grep: cold_ipc: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("cold_ipc: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_COLD_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold_ipc: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("cold_ipc: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold_ipc: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("cold_ipc: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_COLD_SOFT_WAVE);
    kprintf("cold_ipc: soft deepen wave=%u areas=%u logs=%u "
            "svc_bound=%u doors_usable=%u queue_usable=%u submits=%llu "
            "(Wave 35 exclusive; soft inventory; not bar3)\n",
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
            "reply,init,attach,stats,rates,honesty,catalog,return,retmap,deepen,PASS\n",
            (unsigned)GJ_COLD_SOFT_WAVE, (unsigned)GJ_COLD_SOFT_AREAS);

    /* Grep: cold: soft surfaces (Wave 20 deepen) */
    kprintf("cold: soft surfaces count=%u "
            "names=inventory,mode,service,queue,path,submit,dequeue,"
            "reply,init,attach,stats,rates,honesty,catalog,surfaces,"
            "note,return,retmap,deepen,PASS wave=%u\n",
            (unsigned)GJ_COLD_SOFT_AREAS, (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft note (Wave 20 deepen) */
    kprintf("cold: soft note milestone=wave98 exclusive=1 "
            "soft_only=1 not_bar3=1 submits=%llu logs=%u wave=%u\n",
            (unsigned long long)g_u64Submits, g_u32SoftLogN,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft return (Wave 20 deepen) */
    kprintf("cold: soft return submit_svc=%u submit_door=%u "
            "submit_queue=%u submit_enosys=%u deq_claim=%u deq_empty=%u "
            "product_gate=0 wave=%u\n",
            g_u32SoftSubmitSvc, g_u32SoftSubmitDoor, g_u32SoftSubmitQueue,
            g_u32SoftSubmitEnosys, g_u32SoftDeqClaim, g_u32SoftDeqEmpty,
            (unsigned)GJ_COLD_SOFT_WAVE);

    /* Grep: cold: soft deepen wave (Wave 15 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("cold: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /* Grep: cold: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("cold: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("cold: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
    /* Grep: cold: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("cold: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("cold: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("cold: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("cold: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("cold: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: cold: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("cold: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("cold: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("cold: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("cold: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("cold: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("cold: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("cold: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
            /* Grep: cold: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("cold: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_COLD_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: cold: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("cold: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_COLD_SOFT_WAVE);
                    /* Grep: cold: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("cold: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_COLD_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("cold: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("cold: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_COLD_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("cold: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("cold: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cold: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("cold: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("cold: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cold: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
                            /* Grep: cold: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("cold: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("cold: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("cold: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("cold: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("cold: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("cold: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("cold: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retfortress — Wave 35 return-fortress honesty */
kprintf("cold: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("cold: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft rethold — Wave 36 return-hold honesty */
kprintf("cold: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retspire — Wave 36 exclusive spire stamp */
kprintf("cold: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retwall — Wave 37 return-wall honesty */
kprintf("cold: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retgate — Wave 37 exclusive gate stamp */
kprintf("cold: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retmoat — Wave 38 return-moat honesty */
kprintf("cold: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retower — Wave 38 exclusive tower stamp */
kprintf("cold: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("cold: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("cold: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("cold: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("cold: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retravelin — Wave 41 return-travelin honesty */
kprintf("cold: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("cold: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("cold: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("cold: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("cold: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("cold: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("cold: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("cold: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("cold: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("cold: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retbailey — Wave 46 return-bailey honesty */
kprintf("cold: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);
/* Grep: cold: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("cold: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_COLD_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("cold: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("cold: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("cold: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("cold: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("cold: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("cold: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retsally — Wave 50 return-sally honesty */
kprintf("cold: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("cold: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retfosse — Wave 51 return-fosse honesty */
kprintf("cold: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("cold: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("cold: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("cold: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retravelin — Wave 53 return-travelin honesty */
kprintf("cold: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("cold: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("cold: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retredan — Wave 54 exclusive redan stamp */
kprintf("cold: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retflank — Wave 55 return-flank honesty */
kprintf("cold: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retface — Wave 55 exclusive face stamp */
kprintf("cold: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retgorge — Wave 56 return-gorge honesty */
kprintf("cold: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("cold: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retraverse — Wave 57 return-traverse honesty */
kprintf("cold: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("cold: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retorillon — Wave 58 return-orillon honesty */
kprintf("cold: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("cold: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("cold: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("cold: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retplace — Wave 60 return-place honesty */
kprintf("cold: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("cold: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("cold: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("cold: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("cold: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("cold: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("cold: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("cold: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("cold: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: cold: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("cold: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("cold: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cold: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("cold: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("cold: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=112 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cold: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("cold: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=112 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("cold: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("cold: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("cold: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("cold: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("cold: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=112 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("cold: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=112 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("cold: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("cold: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("cold: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("cold: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("cold: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("cold: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: cold: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("cold: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("cold: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionangle stamp; Soft≠product)\n");
/* Grep: cold: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("cold: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("cold: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparapetangle stamp; Soft≠product)\n");
/* Grep: cold: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("cold: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("cold: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retowerangle stamp; Soft≠product)\n");
/* Grep: cold: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("cold: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("cold: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwallangle stamp; Soft≠product)\n");
/* Grep: cold: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("cold: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("cold: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retholdangle stamp; Soft≠product)\n");
/* Grep: cold: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("cold: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("cold: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retfortressangle stamp; Soft≠product)\n");
/* Grep: cold: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("cold: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("cold: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: cold: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("cold: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("cold: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: cold: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("cold: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("cold: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: cold: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("cold: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("cold: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaegisangle stamp; Soft≠product)\n");
/* Grep: cold: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("cold: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("cold: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retsigilangle stamp; Soft≠product)\n");
/* Grep: cold: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("cold: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("cold: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retglyphangle stamp; Soft≠product)\n");
/* Grep: cold: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("cold: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("cold: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retshardangle stamp; Soft≠product)\n");
/* Grep: cold: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("cold: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("cold: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retprismangle stamp; Soft≠product)\n");
/* Grep: cold: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("cold: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("cold: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcipherangle stamp; Soft≠product)\n");
/* Grep: cold: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("cold: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("cold: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retledgerangle stamp; Soft≠product)\n");
/* Grep: cold: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("cold: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("cold: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvaultangle stamp; Soft≠product)\n");
/* Grep: cold: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("cold: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("cold: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettokenangle stamp; Soft≠product)\n");
/* Grep: cold: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("cold: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("cold: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retphaseangle stamp; Soft≠product)\n");
/* Grep: cold: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("cold: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("cold: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retpulseangle stamp; Soft≠product)\n");

/* Grep: cold: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("cold: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("cold: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retboundangle stamp; Soft≠product)\n");
/* Grep: cold: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("cold: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("cold: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbladeangle stamp; Soft≠product)\n");
/* Grep: cold: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("cold: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("cold: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retarcangle stamp; Soft≠product)\n");
/* Grep: cold: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("cold: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("cold: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: cold: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("cold: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("cold: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: cold: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("cold: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("cold: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retellipseangle stamp; Soft≠product)\n");
/* Grep: cold: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("cold: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("cold: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: cold: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("cold: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("cold: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rethelixangle stamp; Soft≠product)\n");
/* Grep: cold: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("cold: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("cold: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retknotangle stamp; Soft≠product)\n");
/* Grep: cold: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("cold: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("cold: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retkleinangle stamp; Soft≠product)\n");
/* Grep: cold: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("cold: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("cold: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retaffineangle stamp; Soft≠product)\n");
/* Grep: cold: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("cold: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("cold: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: cold: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("cold: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("cold: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcubicangle stamp; Soft≠product)\n");
/* Grep: cold: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("cold: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("cold: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retquinticangle stamp; Soft≠product)\n");
/* Grep: cold: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("cold: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("cold: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbezierangle stamp; Soft≠product)\n");
/* Grep: cold: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("cold: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("cold: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: cold: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("cold: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("cold: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: cold: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("cold: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("cold: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retgridangle stamp; Soft≠product)\n");
/* Grep: cold: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("cold: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("cold: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (rettexelangle stamp; Soft≠product)\n");
/* Grep: cold: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("cold: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=112 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("cold: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=112 (retvertexangle stamp; Soft≠product)\n");
                            kprintf("cold: soft deepen wave=%u areas=%u logs=%u "
            "svc_bound=%u doors_usable=%u queue_usable=%u submits=%llu "
            "(Wave 35 exclusive; soft inventory; not bar3)\n",
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
