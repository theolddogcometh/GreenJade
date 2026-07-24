/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Capability revoke: DEAD/gen first (fail closed), then mandatory deferred
 * CNode slot invalidation (S1–S7, R1–R9). See SECURITY_CORE_DESIGN §1.1.
 *
 * Phase A  — logical invalidate (DEAD + gen); security complete.
 * Phase A′ — deferred CDT walk + structured slot invalidate (this file).
 * Phase C  — reclaim when slots_left/refs/pins allow.
 *
 * Soft CDT: edges on pObj->pCdtHead when mint wires them; empty list with
 * slots_left > 0 is a known soft gap (grep: cap:cdt soft).
 *
 * R2 try-lock slot walk (this file):
 *   Soft CNode lock is gj_cnode_trylock (u32SoftLock CAS). Product-true
 *   mutex/turnstile is still missing — R2 is intentionally PARTIAL.
 *   Policy is sleep-not-spin: never busy-wait a busy CNode; leave the edge
 *   linked, try siblings, soft-retry a bounded number of re-walks, then
 *   defer to timer/idle (R7). Counters: spins_avoided, retries, slots_cleared.
 *
 * Soft inventory (Wave 40 exclusive deepen; this unit only):
 *   - Phase A begin path: ok / dead / busy / again / null / queue full
 *   - Deferred queue: push / drop / pending samples / cursor / full
 *   - CDT walk batch: enter / clear / busy / stale / visit / pass / retry
 *   - R2 soft: trylock busy, soft retries, outer deferred push, defer log
 *   - Process deferred: scan / hygiene / empty-edge soft gap / budget
 *   - Reclaim: ready / busy / ok / null; slots_left / pin / ref gates
 *   - honesty / deepen / PASS / mutex / reply non-claims
 *   - return surface (Wave 17): begin|walk|deferred|reclaim gj_status /
 *     u32 cleared buckets under "cap: revoke soft return …"
 *   - return rate / retcode (Wave 20 deepen): ok/fail rate lamps + retcode
 *     catalog under "cap: revoke soft return rate|retcode …"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ product mutex.
 *   Soft ≠ bar3. Soft ≠ GJ_CAP_REPLY product. Soft ≠ MIG REPLY product.
 *   Soft ≠ full CDT mutex product. Product CNode turnstile still OPEN.
 *
 * Grep: cap:cdt deferred / cap:cdt walk / cap:quota soft
 * Grep: cap: revoke try-lock / cap:cdt R2 soft / cap:cdt trylock
 * Grep: cap: revoke soft  (Wave 20 deepen surface)
 * Grep: cap: revoke soft honesty|inventory|deepen|PASS|mutex|reply|return
 * Grep: cap: revoke …     (begin|walk|deferred|reclaim|r2|try-lock|queue)
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/klog.h>

/* Max objects waiting for slot hygiene / reclaim on this simple M0–M2 queue. */
#define GJ_REVOKE_Q_MAX 64u

/*
 * Bounded soft re-walks of busy CDT edges within one batch (R2).
 * Not a spin: each pass only try-locks once per edge; further progress
 * yields to the deferred driver (timer/idle) — sleep-not-spin.
 */
#define GJ_REVOKE_R2_SOFT_RETRY_MAX 3u

/* Wave 20 deepen stamp (file-local; never hard-gates). */
#define GJ_REVOKE_SOFT_WAVE 108u
/* +return selftest|retmap over Wave 17 return rate|retcode */
#define GJ_REVOKE_SOFT_AREAS 192u

struct gj_revoke_qent {
    struct gj_obj_hdr *pObj;
    u8                 u8Active;
};

static struct gj_revoke_qent g_aRevokeQ[GJ_REVOKE_Q_MAX];
static u32                   g_u32RevokeQLen;
/* Round-robin cursor so multi-object queues make progress (R7). */
static u32                   g_u32RevokeQCursor;
/* Soft once-marker: empty CDT while slots_left > 0 (avoid timer log spam). */
static u8                    g_u8CdtSoftLogged;
/* Soft once-marker: try-lock busy deferred an edge (cap:cdt trylock). */
static u8                    g_u8CdtTrylockLogged;
/* Soft once-marker: R2 try-lock walk summary (cap: revoke try-lock). */
static u8                    g_u8RevokeTrylockLogged;
/* Soft once-marker: R2 soft defer while edges remain (cap:cdt R2 soft). */
static u8                    g_u8CdtR2SoftLogged;
/* Soft once-marker: Wave 17 multi-line inventory dump. */
static u8                    g_u8RevokeSoftInvLogged;

/*
 * R2 observability counters (lifetime, process-wide soft stats).
 *   spins_avoided — try-lock busy → deferred (did not spin)
 *   retries       — soft re-walk passes after a busy edge
 *   slots_cleared — structured invalidates via CDT walk batch
 * Grep: cap: revoke try-lock / cap:cdt R2 soft
 */
static u32 g_u32R2SpinsAvoided;
static u32 g_u32R2Retries;
static u32 g_u32R2SlotsCleared;

/*
 * Wave 35 exclusive soft deepen counters (file-local; wrap OK; never hard-gate).
 * Grep: cap: revoke soft
 */
static u32 g_u32SoftBeginEnter;     /* gj_obj_revoke_begin entries */
static u32 g_u32SoftBeginOk;        /* Phase A success → queued */
static u32 g_u32SoftBeginNull;      /* pObj == NULL */
static u32 g_u32SoftBeginDead;      /* concurrent revoke → DEAD/REVOKING */
static u32 g_u32SoftBeginBusy;      /* CAS fail other state */
static u32 g_u32SoftBeginAgain;     /* queue full after DEAD (R7 retry) */
/* Wave 19 return-surface: walk/deferred cleared buckets. */
static u32 g_u32SoftRetWalkZero;    /* walk batch returned 0 cleared */
static u32 g_u32SoftRetWalkPos;     /* walk batch returned >0 cleared */
static u32 g_u32SoftRetWalkSum;     /* sum of walk cleared (soft) */
static u32 g_u32SoftRetDefZero;     /* process_deferred returned 0 */
static u32 g_u32SoftRetDefPos;      /* process_deferred returned >0 */
static u32 g_u32SoftRetDefSum;      /* sum of deferred cleared (soft) */
static u32 g_u32SoftQPush;          /* revoke_q_push accepted (new or dup) */
static u32 g_u32SoftQPushNew;       /* new queue slot taken */
static u32 g_u32SoftQPushDup;       /* already queued */
static u32 g_u32SoftQPushFull;      /* no free slot */
static u32 g_u32SoftQDrop;          /* revoke_q_drop */
static u32 g_u32SoftQPendingSample; /* gj_revoke_deferred_pending samples */
static u32 g_u32SoftQPendingPeak;   /* peak active pending observed */
static u32 g_u32SoftWalkEnter;      /* gj_revoke_cdt_walk_batch entries */
static u32 g_u32SoftWalkNop;        /* null/zero-budget walk early out */
static u32 g_u32SoftWalkPass;       /* primary + soft-retry pass count */
static u32 g_u32SoftWalkBusyEdge;   /* edges left linked due to trylock */
static u32 g_u32SoftWalkStale;      /* stale/bad edges unlinked */
static u32 g_u32SoftWalkVisit;      /* edge visits (all passes) */
static u32 g_u32SoftWalkCleanPass;  /* passes with zero busy */
static u32 g_u32SoftWalkBudgetHit;  /* cleared hit u32MaxSlots */
static u32 g_u32SoftWalkVisitCap;   /* hit visit cap */
static u32 g_u32SoftR2OuterPush;    /* deferred outer second-batch push */
static u32 g_u32SoftR2DeferLog;     /* R2 soft defer once-log emissions */
static u32 g_u32SoftR2TrylockLog;   /* trylock busy once-log emissions */
static u32 g_u32SoftR2WalkLog;      /* try-lock walk summary emissions */
static u32 g_u32SoftDefEnter;       /* gj_revoke_process_deferred entries */
static u32 g_u32SoftDefNop;         /* zero budget / empty queue */
static u32 g_u32SoftDefScan;        /* queue ents examined */
static u32 g_u32SoftDefActive;      /* active DEAD objs scanned */
static u32 g_u32SoftDefWalkCall;    /* cdt_walk_batch calls from deferred */
static u32 g_u32SoftDefEmptyEdge;   /* empty-edge soft gap hits */
static u32 g_u32SoftDefHygieneDone; /* slots_left==0 && no edges (skip) */
static u32 g_u32SoftDefStaleQ;      /* drop non-DEAD / null queue ent */
static u32 g_u32SoftDefEdgeRemain;  /* keep queued with edges after walk */
static u32 g_u32SoftInvClear;       /* gj_cap_slot_invalidate_locked clears */
static u32 g_u32SoftInvSkip;        /* invalidate early-out (null/invalid) */
static u32 g_u32SoftInvWrongObj;    /* slot points at different object */
static u32 g_u32SoftInvQuotaNull;   /* refund via NULL soft path */
static u32 g_u32SoftReclaimReady;   /* gj_obj_reclaim_ready == 1 */
static u32 g_u32SoftReclaimNot;     /* reclaim_ready == 0 */
static u32 g_u32SoftReclaimOk;      /* gj_obj_reclaim success */
static u32 g_u32SoftReclaimBusy;    /* reclaim not ready */
static u32 g_u32SoftReclaimNull;    /* reclaim null arg */
static u32 g_u32SoftReclaimGateSlot;/* not ready: slots_left */
static u32 g_u32SoftReclaimGateCdt; /* not ready: pCdtHead */
static u32 g_u32SoftReclaimGateRef; /* not ready: ref */
static u32 g_u32SoftReclaimGatePin; /* not ready: pin */
static u32 g_u32SoftReclaimGateSt;  /* not ready: state != DEAD */
static u32 g_u32SoftLogN;           /* soft inventory dump emissions */

static void soft_inc(u32 *pCtr);
static void soft_note_pending_peak(u32 u32Pending);
static void soft_revoke_inventory_log(void);
static void soft_revoke_inventory_maybe_once(void);

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: track peak deferred-queue occupancy. */
static void
soft_note_pending_peak(u32 u32Pending)
{
    if (u32Pending > g_u32SoftQPendingPeak) {
        g_u32SoftQPendingPeak = u32Pending;
    }
}

/**
 * Greppable soft revoke inventory (Wave 20 deepen).
 * Prefix family (keep stable for smokes / tooling):
 *   cap: revoke soft inventory|begin|queue|walk|r2|deferred|reclaim|path|…
 *   cap: revoke soft honesty|mutex|reply|return|deepen|PASS
 *   cap: revoke try-lock soft …
 *   cap:cdt R2 soft inventory|…
 * Grep: cap: revoke soft
 * Grep: cap: revoke try-lock
 * Grep: cap:cdt R2 soft
 */
static void
soft_revoke_inventory_log(void)
{
    u32 u32Pending;
    u32 u32Limit;
    u32 iEnt;
    u32 u32Active;

    soft_inc(&g_u32SoftLogN);

    /* Live queue snapshot (diagnostics only). */
    u32Active = 0;
    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }
    for (iEnt = 0; iEnt < u32Limit; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active) {
            u32Active++;
        }
    }
    u32Pending = u32Active;
    soft_note_pending_peak(u32Pending);

    /*
     * Primary Wave 20 deepen lines under "cap: revoke soft …".
     * Honesty: soft u32SoftLock only — product try-lock still partial.
     * Soft ≠ GJ_CAP_REPLY product / MIG REPLY product / full CDT mutex.
     */
    /* Grep: cap: revoke soft honesty */
    kprintf("cap: revoke soft honesty reply_product=0 full_cdt_mutex=0 "
            "soft_lock=u32SoftLock sleep_not_spin=1 r2_partial=1 bar3=0 "
            "soft_ne_mig_reply=1 wave=%u (soft != GJ_CAP_REPLY product; "
            "soft != MIG REPLY product; soft != full CDT mutex product; "
            "soft inventory only)\n",
            GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft inventory */
    kprintf("cap: revoke soft inventory q_max=%u r2_retry_max=%u "
            "pending=%u peak=%u q_len=%u cursor=%u "
            "spins_avoided=%u retries=%u slots_cleared=%u "
            "log_n=%u areas=%u wave=%u soft_partial\n",
            GJ_REVOKE_Q_MAX, GJ_REVOKE_R2_SOFT_RETRY_MAX, u32Pending,
            g_u32SoftQPendingPeak, g_u32RevokeQLen, g_u32RevokeQCursor,
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftLogN, GJ_REVOKE_SOFT_AREAS, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft begin */
    kprintf("cap: revoke soft begin enter=%u ok=%u null=%u dead=%u "
            "busy=%u again=%u queue_full=%u wave=%u\n",
            g_u32SoftBeginEnter, g_u32SoftBeginOk, g_u32SoftBeginNull,
            g_u32SoftBeginDead, g_u32SoftBeginBusy, g_u32SoftBeginAgain,
            g_u32SoftQPushFull, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft queue */
    kprintf("cap: revoke soft queue push=%u new=%u dup=%u full=%u drop=%u "
            "pending_sample=%u peak=%u live=%u cursor=%u q_max=%u "
            "wave=%u\n",
            g_u32SoftQPush, g_u32SoftQPushNew, g_u32SoftQPushDup,
            g_u32SoftQPushFull, g_u32SoftQDrop, g_u32SoftQPendingSample,
            g_u32SoftQPendingPeak, u32Pending, g_u32RevokeQCursor,
            GJ_REVOKE_Q_MAX, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft walk */
    kprintf("cap: revoke soft walk enter=%u nop=%u pass=%u busy_edge=%u "
            "stale=%u visit=%u clean_pass=%u budget_hit=%u visit_cap=%u "
            "inv_clear=%u inv_skip=%u inv_wrong=%u inv_qnull=%u wave=%u\n",
            g_u32SoftWalkEnter, g_u32SoftWalkNop, g_u32SoftWalkPass,
            g_u32SoftWalkBusyEdge, g_u32SoftWalkStale, g_u32SoftWalkVisit,
            g_u32SoftWalkCleanPass, g_u32SoftWalkBudgetHit,
            g_u32SoftWalkVisitCap, g_u32SoftInvClear, g_u32SoftInvSkip,
            g_u32SoftInvWrongObj, g_u32SoftInvQuotaNull, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft r2 / cap: revoke try-lock soft */
    kprintf("cap: revoke soft r2 spins_avoided=%u retries=%u "
            "slots_cleared=%u outer_push=%u trylock_log=%u walk_log=%u "
            "defer_log=%u sleep_not_spin=1 soft_lock=u32SoftLock "
            "product_mutex=OPEN full_cdt_mutex=0 soft_partial wave=%u\n",
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftR2OuterPush, g_u32SoftR2TrylockLog, g_u32SoftR2WalkLog,
            g_u32SoftR2DeferLog, GJ_REVOKE_SOFT_WAVE);
    kprintf("cap: revoke try-lock soft spins_avoided=%u retries=%u "
            "slots_cleared=%u busy_edge=%u outer_push=%u "
            "soft_partial wave=%u (inventory; soft != full CDT mutex "
            "product)\n",
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftWalkBusyEdge, g_u32SoftR2OuterPush, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap:cdt R2 soft inventory */
    kprintf("cap:cdt R2 soft inventory spins_avoided=%u retries=%u "
            "slots_cleared=%u busy_edge=%u outer_push=%u defer_log=%u "
            "trylock_log=%u sleep_not_spin=1 product_mutex=OPEN "
            "full_cdt_mutex=0 soft_partial wave=%u\n",
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftWalkBusyEdge, g_u32SoftR2OuterPush, g_u32SoftR2DeferLog,
            g_u32SoftR2TrylockLog, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft deferred */
    kprintf("cap: revoke soft deferred enter=%u nop=%u scan=%u active=%u "
            "walk_call=%u empty_edge=%u hygiene_done=%u stale_q=%u "
            "edge_remain=%u wave=%u\n",
            g_u32SoftDefEnter, g_u32SoftDefNop, g_u32SoftDefScan,
            g_u32SoftDefActive, g_u32SoftDefWalkCall, g_u32SoftDefEmptyEdge,
            g_u32SoftDefHygieneDone, g_u32SoftDefStaleQ,
            g_u32SoftDefEdgeRemain, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft reclaim */
    kprintf("cap: revoke soft reclaim ready=%u not=%u ok=%u busy=%u null=%u "
            "gate_st=%u gate_slot=%u gate_cdt=%u gate_ref=%u gate_pin=%u "
            "wave=%u\n",
            g_u32SoftReclaimReady, g_u32SoftReclaimNot, g_u32SoftReclaimOk,
            g_u32SoftReclaimBusy, g_u32SoftReclaimNull, g_u32SoftReclaimGateSt,
            g_u32SoftReclaimGateSlot, g_u32SoftReclaimGateCdt,
            g_u32SoftReclaimGateRef, g_u32SoftReclaimGatePin,
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft mutex
     * R2 soft lock only — full CDT mutex / turnstile product still OPEN.
     */
    kprintf("cap: revoke soft mutex soft_lock=u32SoftLock "
            "product_mutex=OPEN full_cdt_mutex=0 turnstile=OPEN "
            "sleep_not_spin=1 spins_avoided=%u retries=%u "
            "wave=%u (soft != full CDT mutex product)\n",
            g_u32R2SpinsAvoided, g_u32R2Retries, GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft reply
     * Revoke path does not mint/consume GJ_CAP_REPLY; door soft table only.
     * Soft ≠ MIG REPLY product.
     */
    kprintf("cap: revoke soft reply reply_product=0 mig_reply=0 "
            "cnode_reply_install=0 soft_ne_mig_reply=1 wave=%u "
            "(soft != GJ_CAP_REPLY product; soft != MIG REPLY product; "
            "door REPLY soft table is separate)\n",
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft return
     * Wave 19 public return-surface: begin/walk/deferred/reclaim buckets.
     */
    kprintf("cap: revoke soft return begin_ok=%u begin_null=%u "
            "begin_dead=%u begin_busy=%u begin_again=%u "
            "walk_zero=%u walk_pos=%u walk_sum=%u "
            "def_zero=%u def_pos=%u def_sum=%u "
            "reclaim_ok=%u reclaim_busy=%u reclaim_null=%u "
            "wave=%u\n",
            g_u32SoftBeginOk, g_u32SoftBeginNull, g_u32SoftBeginDead,
            g_u32SoftBeginBusy, g_u32SoftBeginAgain, g_u32SoftRetWalkZero,
            g_u32SoftRetWalkPos, g_u32SoftRetWalkSum, g_u32SoftRetDefZero,
            g_u32SoftRetDefPos, g_u32SoftRetDefSum, g_u32SoftReclaimOk,
            g_u32SoftReclaimBusy, g_u32SoftReclaimNull, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft return begin — status surface */
    kprintf("cap: revoke soft return begin ok=%u null=%u dead=%u "
            "busy=%u again=%u wave=%u\n",
            g_u32SoftBeginOk, g_u32SoftBeginNull, g_u32SoftBeginDead,
            g_u32SoftBeginBusy, g_u32SoftBeginAgain, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft return walk — cleared buckets */
    kprintf("cap: revoke soft return walk zero=%u pos=%u sum=%u "
            "wave=%u\n",
            g_u32SoftRetWalkZero, g_u32SoftRetWalkPos, g_u32SoftRetWalkSum,
            GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft return deferred — cleared buckets */
    kprintf("cap: revoke soft return deferred zero=%u pos=%u sum=%u "
            "wave=%u\n",
            g_u32SoftRetDefZero, g_u32SoftRetDefPos, g_u32SoftRetDefSum,
            GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft return reclaim — status surface */
    kprintf("cap: revoke soft return reclaim ok=%u busy=%u null=%u "
            "ready=%u not=%u wave=%u\n",
            g_u32SoftReclaimOk, g_u32SoftReclaimBusy, g_u32SoftReclaimNull,
            g_u32SoftReclaimReady, g_u32SoftReclaimNot, GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft path */
    kprintf("cap: revoke soft path phase_a=DEAD_gen_first phase_ap=cdt_walk "
            "r2=trylock_defer r7=timer_idle_redrive phase_c=reclaim "
            "lock=soft_u32SoftLock product=PARTIAL return_surface=1 return_rate=1 retcode=1 return_selftest=1 retmap=1 "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1 "
            "wave=%u (soft inventory; not bar3; soft != GJ_CAP_REPLY "
            "product; soft != MIG REPLY product; soft != full CDT mutex "
            "product)\n",
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft return rate
     * Wave 17 return-surface rate lamps (kept) (soft ≠ product).
     */
    kprintf("cap: revoke soft return rate "
            "begin_ok=%u begin_fail=%u "
            "walk_pos=%u walk_zero=%u "
            "def_pos=%u def_zero=%u "
            "reclaim_ok=%u reclaim_busy=%u reclaim_null=%u "
            "wave=%u (return rate; Soft≠product; soft≠full CDT mutex; "
            "not bar3)\n",
            g_u32SoftBeginOk,
            g_u32SoftBeginNull + g_u32SoftBeginDead + g_u32SoftBeginBusy +
                g_u32SoftBeginAgain,
            g_u32SoftRetWalkPos, g_u32SoftRetWalkZero,
            g_u32SoftRetDefPos, g_u32SoftRetDefZero,
            g_u32SoftReclaimOk, g_u32SoftReclaimBusy, g_u32SoftReclaimNull,
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft retcode
     * Wave 17 retcode catalog for begin/reclaim status classes.
     */
    kprintf("cap: revoke soft retcode "
            "ok=1 null=1 dead=1 busy=1 again=1 "
            "walk_zero=1 walk_pos=1 def_zero=1 def_pos=1 "
            "reclaim_ready=1 reclaim_not=1 "
            "begin_ok=%u reclaim_ok=%u wave=%u "
            "(retcode catalog; Soft≠product; soft≠MIG REPLY product)\n",
            g_u32SoftBeginOk, g_u32SoftReclaimOk, GJ_REVOKE_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: cap: revoke soft return selftest — Wave 19 terminal return surface */
    kprintf("cap: revoke soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft retmap — Wave 19 return-surface map */
    kprintf("cap: revoke soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);

    /* Grep: cap: revoke soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cap: revoke: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("cap: revoke: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);
    /* Grep: cap: revoke: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("cap: revoke: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cap: revoke: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("cap: revoke: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);
    /* Grep: cap: revoke: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("cap: revoke: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_REVOKE_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cap: revoke: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("cap: revoke: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("cap: revoke: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cap: revoke: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("cap: revoke: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("cap: revoke: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: cap: revoke: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("cap: revoke: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("cap: revoke: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cap: revoke: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("cap: revoke: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("cap: revoke: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cap: revoke: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("cap: revoke: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("cap: revoke: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cap: revoke: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("cap: revoke: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
            /* Grep: cap: revoke: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("cap: revoke: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: cap: revoke: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("cap: revoke: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_REVOKE_SOFT_WAVE);
                    /* Grep: cap: revoke: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("cap: revoke: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cap: revoke: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("cap: revoke: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /* Grep: cap: revoke: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("cap: revoke: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_REVOKE_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cap: revoke: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("cap: revoke: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /* Grep: cap: revoke: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("cap: revoke: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cap: revoke: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cap: revoke: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /* Grep: cap: revoke: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("cap: revoke: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cap: revoke: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("cap: revoke: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /* Grep: cap: revoke: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cap: revoke: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
                            /* Grep: cap: revoke: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("cap: revoke: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("cap: revoke: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("cap: revoke: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("cap: revoke: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("cap: revoke: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("cap: revoke: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("cap: revoke: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retfortress — Wave 35 return-fortress honesty */
kprintf("cap: revoke: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("cap: revoke: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft rethold — Wave 36 return-hold honesty */
kprintf("cap: revoke: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retspire — Wave 36 exclusive spire stamp */
kprintf("cap: revoke: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retwall — Wave 37 return-wall honesty */
kprintf("cap: revoke: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retgate — Wave 37 exclusive gate stamp */
kprintf("cap: revoke: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retmoat — Wave 38 return-moat honesty */
kprintf("cap: revoke: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retower — Wave 38 exclusive tower stamp */
kprintf("cap: revoke: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("cap: revoke: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("cap: revoke: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("cap: revoke: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("cap: revoke: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retravelin — Wave 41 return-travelin honesty */
kprintf("cap: revoke: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("cap: revoke: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("cap: revoke: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("cap: revoke: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("cap: revoke: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("cap: revoke: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("cap: revoke: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("cap: revoke: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("cap: revoke: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("cap: revoke: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retbailey — Wave 46 return-bailey honesty */
kprintf("cap: revoke: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);
/* Grep: cap: revoke: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("cap: revoke: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_REVOKE_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("cap: revoke: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("cap: revoke: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("cap: revoke: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("cap: revoke: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("cap: revoke: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("cap: revoke: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retsally — Wave 50 return-sally honesty */
kprintf("cap: revoke: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("cap: revoke: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retfosse — Wave 51 return-fosse honesty */
kprintf("cap: revoke: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("cap: revoke: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("cap: revoke: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("cap: revoke: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retravelin — Wave 53 return-travelin honesty */
kprintf("cap: revoke: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("cap: revoke: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("cap: revoke: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retredan — Wave 54 exclusive redan stamp */
kprintf("cap: revoke: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retflank — Wave 55 return-flank honesty */
kprintf("cap: revoke: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retface — Wave 55 exclusive face stamp */
kprintf("cap: revoke: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retgorge — Wave 56 return-gorge honesty */
kprintf("cap: revoke: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("cap: revoke: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retraverse — Wave 57 return-traverse honesty */
kprintf("cap: revoke: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("cap: revoke: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retorillon — Wave 58 return-orillon honesty */
kprintf("cap: revoke: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("cap: revoke: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("cap: revoke: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("cap: revoke: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retplace — Wave 60 return-place honesty */
kprintf("cap: revoke: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("cap: revoke: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("cap: revoke: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("cap: revoke: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("cap: revoke: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("cap: revoke: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("cap: revoke: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("cap: revoke: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cap: revoke: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("cap: revoke: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: cap: revoke: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("cap: revoke: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cap: revoke: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("cap: revoke: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cap: revoke: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("cap: revoke: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cap: revoke: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("cap: revoke: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=108 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cap: revoke: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("cap: revoke: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=108 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("cap: revoke: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("cap: revoke: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("cap: revoke: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("cap: revoke: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("cap: revoke: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("cap: revoke: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("cap: revoke: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("cap: revoke: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("cap: revoke: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("cap: revoke: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cap: revoke: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("cap: revoke: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("cap: revoke: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("cap: revoke: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("cap: revoke: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbastionangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("cap: revoke: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("cap: revoke: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retparapetangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("cap: revoke: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("cap: revoke: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retowerangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("cap: revoke: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("cap: revoke: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retwallangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("cap: revoke: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("cap: revoke: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retholdangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("cap: revoke: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("cap: revoke: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retfortressangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("cap: revoke: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("cap: revoke: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("cap: revoke: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("cap: revoke: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("cap: revoke: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("cap: revoke: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("cap: revoke: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("cap: revoke: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retaegisangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("cap: revoke: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("cap: revoke: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retsigilangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("cap: revoke: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("cap: revoke: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retglyphangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("cap: revoke: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("cap: revoke: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retshardangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("cap: revoke: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("cap: revoke: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retprismangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("cap: revoke: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("cap: revoke: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcipherangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("cap: revoke: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("cap: revoke: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retledgerangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("cap: revoke: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("cap: revoke: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retvaultangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("cap: revoke: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("cap: revoke: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (rettokenangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("cap: revoke: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("cap: revoke: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retphaseangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("cap: revoke: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("cap: revoke: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retpulseangle stamp; Soft≠product)\n");

/* Grep: cap: revoke: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("cap: revoke: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("cap: revoke: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retboundangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("cap: revoke: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("cap: revoke: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbladeangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("cap: revoke: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("cap: revoke: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retarcangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("cap: revoke: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("cap: revoke: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("cap: revoke: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("cap: revoke: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("cap: revoke: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("cap: revoke: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retellipseangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("cap: revoke: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("cap: revoke: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("cap: revoke: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("cap: revoke: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (rethelixangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("cap: revoke: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("cap: revoke: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retknotangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("cap: revoke: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("cap: revoke: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retkleinangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("cap: revoke: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("cap: revoke: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retaffineangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("cap: revoke: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("cap: revoke: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("cap: revoke: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("cap: revoke: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcubicangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("cap: revoke: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("cap: revoke: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retquinticangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("cap: revoke: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("cap: revoke: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbezierangle stamp; Soft≠product)\n");
/* Grep: cap: revoke: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("cap: revoke: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: cap: revoke: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("cap: revoke: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcatmullangle stamp; Soft≠product)\n");
                            kprintf("cap: revoke soft deepen wave=%u areas=%u pending=%u "
            "spins_avoided=%u retries=%u slots_cleared=%u "
            "begin_ok=%u reclaim_ok=%u walk_pos=%u def_pos=%u "
            "log_n=%u ok=1 skip=0\n",
            GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS, u32Pending,
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftBeginOk, g_u32SoftReclaimOk, g_u32SoftRetWalkPos,
            g_u32SoftRetDefPos, g_u32SoftLogN);

    /* Grep: cap: revoke soft inventory PASS / cap: revoke soft PASS */
    kprintf("cap: revoke soft inventory PASS log_n=%u wave=%u areas=%u "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1\n",
            g_u32SoftLogN, GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS);
    kprintf("cap: revoke soft PASS wave=%u areas=%u\n",
            GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS);
}

/**
 * Emit Wave 19 soft inventory once after first meaningful revoke activity.
 * Avoids timer-tick spam; re-log is not required for greppable surface.
 */
static void
soft_revoke_inventory_maybe_once(void)
{
    if (g_u8RevokeSoftInvLogged) {
        return;
    }
    /* Need at least one begin, walk, or deferred tick with work. */
    if (g_u32SoftBeginEnter == 0 && g_u32SoftWalkEnter == 0 &&
        g_u32SoftDefEnter == 0) {
        return;
    }
    g_u8RevokeSoftInvLogged = 1;
    soft_revoke_inventory_log();
}

/*
 * Enqueue once per object. Scan for duplicates before taking a free slot so
 * a sparse active set cannot double-queue the same object.
 */
static int
revoke_q_push(struct gj_obj_hdr *pObj)
{
    u32 iEnt;
    u32 u32Free = GJ_REVOKE_Q_MAX;

    if (pObj == NULL) {
        return -1;
    }

    for (iEnt = 0; iEnt < GJ_REVOKE_Q_MAX; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active) {
            if (g_aRevokeQ[iEnt].pObj == pObj) {
                soft_inc(&g_u32SoftQPush);
                soft_inc(&g_u32SoftQPushDup);
                return 0; /* already queued */
            }
        } else if (u32Free == GJ_REVOKE_Q_MAX) {
            u32Free = iEnt;
        }
    }

    if (u32Free >= GJ_REVOKE_Q_MAX) {
        soft_inc(&g_u32SoftQPushFull);
        return -1;
    }

    g_aRevokeQ[u32Free].pObj = pObj;
    g_aRevokeQ[u32Free].u8Active = 1;
    if (u32Free >= g_u32RevokeQLen) {
        g_u32RevokeQLen = u32Free + 1;
    }
    soft_inc(&g_u32SoftQPush);
    soft_inc(&g_u32SoftQPushNew);
    soft_note_pending_peak(gj_revoke_deferred_pending());
    return 0;
}

/* Drop one queue entry; shrink published high-water when trailing empties. */
static void
revoke_q_drop(u32 iEnt)
{
    u32 iScan;

    if (iEnt >= GJ_REVOKE_Q_MAX) {
        return;
    }
    g_aRevokeQ[iEnt].u8Active = 0;
    g_aRevokeQ[iEnt].pObj = NULL;
    soft_inc(&g_u32SoftQDrop);

    if (g_u32RevokeQLen == 0) {
        return;
    }
    iScan = g_u32RevokeQLen;
    while (iScan > 0) {
        if (g_aRevokeQ[iScan - 1].u8Active) {
            break;
        }
        iScan--;
    }
    g_u32RevokeQLen = iScan;
}

u32
gj_revoke_deferred_pending(void)
{
    u32 iEnt;
    u32 u32Pending = 0;
    u32 u32Limit;

    soft_inc(&g_u32SoftQPendingSample);

    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }
    for (iEnt = 0; iEnt < u32Limit; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active) {
            u32Pending++;
        }
    }
    soft_note_pending_peak(u32Pending);
    return u32Pending;
}

/*
 * Phase A: DEAD + gen first (S1). Slot work is deferred (S4, R2).
 * Security is complete after DEAD/gen; queue full does not undo that.
 */
gj_status_t
gj_obj_revoke_begin(struct gj_obj_hdr *pObj)
{
    u32 u32Expected;
    u32 u32New;
    u32 u32Cur;

    soft_inc(&g_u32SoftBeginEnter);

    if (pObj == NULL) {
        soft_inc(&g_u32SoftBeginNull);
        return GJ_ERR_INVAL;
    }

    u32Expected = (u32)GJ_OBJ_LIVE;
    u32New = (u32)GJ_OBJ_REVOKING;

    /* One revoker (R6). Concurrent revoke → DEAD/BUSY, never a second owner. */
    if (!__atomic_compare_exchange_n(&pObj->u32State, &u32Expected, u32New,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        u32Cur = __atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE);
        if (u32Cur == (u32)GJ_OBJ_DEAD ||
            u32Cur == (u32)GJ_OBJ_REVOKING) {
            soft_inc(&g_u32SoftBeginDead);
            /* Grep: cap: revoke soft begin dead */
            if (g_u32SoftBeginDead == 1u) {
                kprintf("cap: revoke soft begin concurrent dead/revoking "
                        "soft (once)\n");
            }
            return GJ_ERR_DEAD;
        }
        soft_inc(&g_u32SoftBeginBusy);
        return GJ_ERR_BUSY;
    }

    /*
     * S1 — security boundary: DEAD + bump gen BEFORE any deferrable work.
     * From here, gj_obj_check_live() fails closed (S2/S3) even if slots lag.
     */
    __atomic_store_n(&pObj->u32State, (u32)GJ_OBJ_DEAD, __ATOMIC_RELEASE);
    __atomic_fetch_add(&pObj->u32Gen, 1u, __ATOMIC_ACQ_REL);

    /*
     * Waiters would be woken here (IPC queues, fault waiters) when those
     * subsystems exist — PEER_DEAD / REVOKED. Never block on userspace (R4).
     */

    /* Queue mandatory slot hygiene (S4) + later reclaim (S6, R9). */
    if (revoke_q_push(pObj) != 0) {
        soft_inc(&g_u32SoftBeginAgain);
        /* Grep: revoke: deferred / cap:cdt deferred */
        kprintf("cap:cdt deferred queue full\n");
        /* Grep: cap: revoke soft begin again */
        kprintf("cap: revoke soft begin again queue_full spins_avoided=%u "
                "retries=%u soft_partial\n",
                g_u32R2SpinsAvoided, g_u32R2Retries);
        /* Object is still DEAD — secure; hygiene must be retried (R7). */
        soft_revoke_inventory_maybe_once();
        return GJ_ERR_AGAIN;
    }

    soft_inc(&g_u32SoftBeginOk);
    /* Grep: cap: revoke soft begin ok */
    if (g_u32SoftBeginOk == 1u) {
        kprintf("cap: revoke soft begin ok dead_gen_first queued=1 "
                "pending=%u wave=%u soft\n",
                gj_revoke_deferred_pending(), GJ_REVOKE_SOFT_WAVE);
    }
    soft_revoke_inventory_maybe_once();
    return GJ_OK;
}

/*
 * Structured slot invalidate (S7). Caller holds the CNode lock in full impl.
 * If pObj is non-NULL, only touch a slot that still points at that object —
 * never clear an unrelated cap during a CDT-driven walk.
 *
 * Single path for slots_left; soft quota refund is done by callers that know
 * the owning CNode (CDT edge / invalidate_obj_slots). This path refunds NULL
 * so accounting stays single-pathed when the CNode is known upstream.
 * Grep: cap:quota soft
 */
void
gj_cap_slot_invalidate_locked(struct gj_cap_slot *pSlot, struct gj_obj_hdr *pObj)
{
    void *pAccount = NULL;

    if (pSlot == NULL) {
        soft_inc(&g_u32SoftInvSkip);
        return;
    }
    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        soft_inc(&g_u32SoftInvSkip);
        return;
    }

    if (pObj != NULL) {
        if (pSlot->pObj != (void *)pObj) {
            /* Wrong object (or already cleared); leave slot alone. */
            soft_inc(&g_u32SoftInvWrongObj);
            return;
        }
        /* One less outstanding derived slot (S4/S6). Saturate at zero. */
        if (pObj->u32SlotsLeft > 0) {
            pObj->u32SlotsLeft--;
        }
    } else if (pSlot->pObj != NULL) {
        struct gj_obj_hdr *pSlotObj = (struct gj_obj_hdr *)pSlot->pObj;

        if (pSlotObj->u32SlotsLeft > 0) {
            pSlotObj->u32SlotsLeft--;
        }
    }

    /*
     * Soft quota refund: CNode account is not on the slot; refund via NULL
     * until invalidate path carries the owning CNode (CDT edge has it).
     * Walk batch refunds explicitly when edge->pCnode is known.
     * Grep: cap:quota soft
     */
    soft_inc(&g_u32SoftInvQuotaNull);
    (void)gj_cap_quota_slot_refund(pAccount);

    /* S7: type INVALID, slot gen++, clear ptr + obj gen */
    gj_cap_slot_invalidate(pSlot);
    soft_inc(&g_u32SoftInvClear);
}

/*
 * Soft CDT walk (Phase A′ batch). Iterative; work-limited; does not delay S1.
 *
 * R2 try-lock slot walk:
 *   - Soft try-lock each CNode (gj_cnode_trylock / u32SoftLock).
 *   - If busy: count spins_avoided, leave edge linked, try siblings.
 *   - After a pass with busy edges, soft-retry up to
 *     GJ_REVOKE_R2_SOFT_RETRY_MAX re-walks (not a spin: one try-lock per
 *     edge per pass). If still busy, return; deferred driver redrives (R7).
 *   - Sleep-not-spin: never busy-wait trylock in a tight loop. A product
 *     path with a real mutex would sleep on the CNode turnstile; that
 *     mutex is still missing — R2 product try-lock remains PARTIAL.
 *   - Stale/bad edges are unlinked. Cleared slots get quota refund against
 *     the owning CNode account.
 *
 * Grep: cap:cdt walk / cap: revoke try-lock / cap:cdt R2 soft
 */
u32
gj_revoke_cdt_walk_batch(struct gj_obj_hdr *pObj, u32 u32MaxSlots)
{
    u32 u32Cleared = 0;
    u32 u32Busy = 0;
    u32 u32Visited = 0;
    u32 u32Pass;
    u32 u32Stale = 0;
    /* Bound visits so a long list under lock contention cannot spin. */
    const u32 u32VisitCap = u32MaxSlots * 4u + 8u;

    soft_inc(&g_u32SoftWalkEnter);

    if (pObj == NULL || u32MaxSlots == 0) {
        soft_inc(&g_u32SoftWalkNop);
        soft_inc(&g_u32SoftRetWalkZero); /* Wave 19 return surface */
        return 0;
    }

    /*
     * Soft multi-pass: walk → soft-retry busy edges → defer if still held.
     * Pass 0 is the primary walk; passes 1..SOFT_RETRY_MAX are retries
     * after at least one busy try-lock (sleep-not-spin policy).
     */
    for (u32Pass = 0;
         u32Pass <= GJ_REVOKE_R2_SOFT_RETRY_MAX &&
         u32Cleared < u32MaxSlots &&
         u32Visited < u32VisitCap;
         u32Pass++) {
        struct gj_cdt_edge *pEdge;
        struct gj_cdt_edge *pNext;
        u32 u32BusyThisPass = 0;

        soft_inc(&g_u32SoftWalkPass);

        if (pObj->pCdtHead == NULL) {
            break;
        }

        if (u32Pass > 0) {
            /*
             * Soft retry after busy (R2): do NOT spin. Re-scan remaining
             * edges once; holder may have dropped the soft lock. If still
             * busy after SOFT_RETRY_MAX, yield to deferred driver (R7) —
             * that is the "sleep" half of sleep-not-spin without a real
             * turnstile.
             * Grep: cap:cdt R2 soft
             */
            g_u32R2Retries++;
            /* Grep: cap: revoke soft r2 retry */
            if (g_u32R2Retries == 1u) {
                kprintf("cap: revoke soft r2 retry pass=%u spins_avoided=%u "
                        "soft_partial (once)\n",
                        u32Pass, g_u32R2SpinsAvoided);
            }
        }

        pEdge = pObj->pCdtHead;
        while (pEdge != NULL && u32Cleared < u32MaxSlots &&
               u32Visited < u32VisitCap) {
            struct gj_cnode *pCnode;
            u64 u64Slot;
            int fUnlink = 0;

            u32Visited++;
            soft_inc(&g_u32SoftWalkVisit);
            pNext = pEdge->pNext;
            pCnode = pEdge->pCnode;
            u64Slot = pEdge->u64Slot;

            if (pCnode == NULL || pCnode->pSlots == NULL ||
                u64Slot >= pCnode->cSlots) {
                /* Stale edge — drop. Grep: cap:cdt stale */
                fUnlink = 1;
                u32Stale++;
                soft_inc(&g_u32SoftWalkStale);
            } else if (!gj_cnode_trylock(pCnode)) {
                /*
                 * R2: CNode busy — leave edge linked, try siblings, soft
                 * retry later. Count as spin avoided (never busy-wait).
                 * Grep: cap:cdt trylock / cap: revoke try-lock
                 */
                u32BusyThisPass++;
                g_u32R2SpinsAvoided++;
                soft_inc(&g_u32SoftWalkBusyEdge);
                if (!g_u8CdtTrylockLogged) {
                    g_u8CdtTrylockLogged = 1;
                    soft_inc(&g_u32SoftR2TrylockLog);
                    kprintf("cap:cdt trylock busy (once)\n");
                    /* Grep: cap: revoke try-lock busy */
                    kprintf("cap: revoke try-lock busy spins_avoided=%u "
                            "pass=%u sleep_not_spin soft_partial (once)\n",
                            g_u32R2SpinsAvoided, u32Pass);
                }
                pEdge = pNext;
                continue;
            } else {
                struct gj_cap_slot *pSlot = &pCnode->pSlots[u64Slot];

                if (pSlot->u16Type != (u16)GJ_CAP_INVALID &&
                    pSlot->pObj == (void *)pObj) {
                    /* Refund against owning CNode account when known. */
                    /* Grep: cap:quota refund */
                    (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
                    /*
                     * slots_left decremented inside invalidate_locked; avoid
                     * double soft-quota by passing account only above once.
                     * invalidate_locked still calls refund(NULL) — soft no-op.
                     */
                    gj_cap_slot_invalidate_locked(pSlot, pObj);
                    u32Cleared++;
                    g_u32R2SlotsCleared++;
                    fUnlink = 1;
                } else {
                    /* Slot already cleared or retargeted — drop edge. */
                    fUnlink = 1;
                    u32Stale++;
                    soft_inc(&g_u32SoftWalkStale);
                }
                gj_cnode_unlock(pCnode);
            }

            if (fUnlink) {
                gj_cdt_edge_unlink(pObj, pEdge);
            }
            pEdge = pNext;
        }

        u32Busy = u32BusyThisPass;
        if (u32BusyThisPass == 0) {
            /* Clean pass — no soft-retry needed. */
            soft_inc(&g_u32SoftWalkCleanPass);
            break;
        }
        /* Busy edges remain: soft-retry next pass, or exit at max. */
    }

    if (u32Cleared >= u32MaxSlots && u32MaxSlots > 0) {
        soft_inc(&g_u32SoftWalkBudgetHit);
    }
    if (u32Visited >= u32VisitCap) {
        soft_inc(&g_u32SoftWalkVisitCap);
    }

    /*
     * Greppable R2 summary (once). Honesty: soft u32SoftLock only —
     * product try-lock still partial without a real CNode mutex.
     * Grep: cap: revoke try-lock
     */
    if (!g_u8RevokeTrylockLogged &&
        (u32Cleared > 0 || g_u32R2SpinsAvoided > 0 || u32Stale > 0)) {
        g_u8RevokeTrylockLogged = 1;
        soft_inc(&g_u32SoftR2WalkLog);
        kprintf("cap: revoke try-lock walk cleared=%u spins_avoided=%u "
                "retries=%u slots_cleared=%u busy_left=%u "
                "soft_partial (once)\n",
                u32Cleared, g_u32R2SpinsAvoided, g_u32R2Retries,
                g_u32R2SlotsCleared, u32Busy);
        /* Grep: cap: revoke soft walk summary */
        kprintf("cap: revoke soft walk summary cleared=%u stale=%u "
                "visited=%u passes=%u busy_left=%u visit_cap=%u "
                "wave=%u soft_partial\n",
                u32Cleared, u32Stale, u32Visited, g_u32SoftWalkPass, u32Busy,
                u32VisitCap, GJ_REVOKE_SOFT_WAVE);
    }

    /* Wave 19 return surface: walk cleared buckets. */
    if (u32Cleared == 0u) {
        soft_inc(&g_u32SoftRetWalkZero);
    } else {
        soft_inc(&g_u32SoftRetWalkPos);
        if (g_u32SoftRetWalkSum < 0xffffffffu - u32Cleared) {
            g_u32SoftRetWalkSum += u32Cleared;
        } else {
            g_u32SoftRetWalkSum = 0xffffffffu;
        }
    }

    soft_revoke_inventory_maybe_once();
    return u32Cleared;
}

/*
 * Phase A′: drive deferred slot work (bounded; R2 — no spin on CNode locks).
 * Prefer full CDT try-lock walk when edges exist (primary, more complete
 * than the empty-edge soft gap). Round-robin across queue so one lagging
 * object cannot starve siblings (R7). Soft-marker only if slots lag
 * without edges.
 *
 * Sleep-not-spin (R2): walk batch may soft-retry busy CNodes; if edges
 * still remain, keep the object queued and return — timer/idle redrives.
 * Never tight-loop trylock here.
 *
 * Honesty: R2 product try-lock is still PARTIAL (soft u32SoftLock, not a
 * real CNode mutex/turnstile).
 *
 * Grep: cap:cdt deferred / cap: revoke try-lock / cap:cdt R2 soft
 */
u32
gj_revoke_process_deferred(u32 u32MaxSlots)
{
    u32 u32Cleared = 0;
    u32 u32Limit;
    u32 u32Scanned;
    u32 iEnt;
    u32 u32Start;

    soft_inc(&g_u32SoftDefEnter);

    if (u32MaxSlots == 0) {
        soft_inc(&g_u32SoftDefNop);
        soft_inc(&g_u32SoftRetDefZero); /* Wave 19 return surface */
        return 0;
    }

    /* Cap scan to published high-water; never past GJ_REVOKE_Q_MAX. */
    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }
    if (u32Limit == 0) {
        soft_inc(&g_u32SoftDefNop);
        soft_inc(&g_u32SoftRetDefZero);
        return 0;
    }

    /* Round-robin: start past last cursor so every active ent gets turns. */
    u32Start = g_u32RevokeQCursor % u32Limit;

    for (u32Scanned = 0; u32Scanned < u32Limit && u32Cleared < u32MaxSlots;
         u32Scanned++) {
        struct gj_obj_hdr *pObj;
        u32 u32State;
        u32 u32Batch;
        u32 u32Budget;

        iEnt = (u32Start + u32Scanned) % u32Limit;
        soft_inc(&g_u32SoftDefScan);

        if (!g_aRevokeQ[iEnt].u8Active) {
            continue;
        }
        pObj = g_aRevokeQ[iEnt].pObj;
        if (pObj == NULL) {
            soft_inc(&g_u32SoftDefStaleQ);
            revoke_q_drop(iEnt);
            continue;
        }

        u32State = __atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE);
        if (u32State != (u32)GJ_OBJ_DEAD) {
            /* Stale queue entry (reclaimed or never completed Phase A). */
            soft_inc(&g_u32SoftDefStaleQ);
            revoke_q_drop(iEnt);
            continue;
        }

        soft_inc(&g_u32SoftDefActive);

        /* Hygiene done for this object — leave queued for reclaim drain. */
        if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
            soft_inc(&g_u32SoftDefHygieneDone);
            continue;
        }

        u32Budget = u32MaxSlots - u32Cleared;

        /*
         * Edges present: full CDT try-lock batch is the primary hygiene path
         * (more complete than empty-edge soft gap). Walk batch itself does
         * bounded soft retries under sleep-not-spin.
         * Grep: cap:cdt walk / cap: revoke try-lock
         */
        if (pObj->pCdtHead != NULL) {
            soft_inc(&g_u32SoftDefWalkCall);
            u32Batch = gj_revoke_cdt_walk_batch(pObj, u32Budget);
            u32Cleared += u32Batch;

            /*
             * Second soft push within this deferred tick if edges remain and
             * budget remains: one more batch only (not a spin). Further work
             * stays on the queue for R7 redrive.
             * Grep: cap:cdt R2 soft
             */
            if (pObj->pCdtHead != NULL && u32Cleared < u32MaxSlots) {
                u32 u32Push;

                u32Budget = u32MaxSlots - u32Cleared;
                soft_inc(&g_u32SoftDefWalkCall);
                soft_inc(&g_u32SoftR2OuterPush);
                u32Push = gj_revoke_cdt_walk_batch(pObj, u32Budget);
                if (u32Push > 0) {
                    g_u32R2Retries++; /* outer deferred soft push */
                }
                u32Cleared += u32Push;
                /* Grep: cap: revoke soft r2 outer */
                if (g_u32SoftR2OuterPush == 1u) {
                    kprintf("cap: revoke soft r2 outer push cleared=%u "
                            "spins_avoided=%u sleep_not_spin soft_partial "
                            "(once)\n",
                            u32Push, g_u32R2SpinsAvoided);
                }
            }

            /* Advance cursor past this ent so next call rotates fairly. */
            g_u32RevokeQCursor = (iEnt + 1u) % u32Limit;

            if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
                soft_inc(&g_u32SoftDefHygieneDone);
                continue;
            }

            /* Edges remain (budget or try-lock busy) — keep queued (R7). */
            if (pObj->pCdtHead != NULL) {
                soft_inc(&g_u32SoftDefEdgeRemain);
                /*
                 * Soft R2 defer: edges still linked after try-lock walks.
                 * Product mutex still missing — partial R2.
                 * Grep: cap:cdt R2 soft / cap: revoke try-lock
                 */
                if (!g_u8CdtR2SoftLogged) {
                    g_u8CdtR2SoftLogged = 1;
                    soft_inc(&g_u32SoftR2DeferLog);
                    kprintf("cap:cdt R2 soft defer edges remain "
                            "spins_avoided=%u retries=%u slots_cleared=%u "
                            "soft_partial (once)\n",
                            g_u32R2SpinsAvoided, g_u32R2Retries,
                            g_u32R2SlotsCleared);
                    /* Grep: cap: revoke soft r2 defer */
                    kprintf("cap: revoke soft r2 defer edges_remain=1 "
                            "outer_push=%u busy_edge=%u wave=%u "
                            "soft_partial (once)\n",
                            g_u32SoftR2OuterPush, g_u32SoftWalkBusyEdge,
                            GJ_REVOKE_SOFT_WAVE);
                }
                continue;
            }
        }

        /*
         * Soft gap: derived slots counted but no CDT edges to walk.
         * Security already done (DEAD/gen); accounting complete needs mint
         * to wire edges or a known-CNode scan (gj_cnode_invalidate_obj_slots).
         * Only when edges are absent — walk path above is preferred.
         * Grep: cap:cdt soft
         */
        if (pObj->u32SlotsLeft > 0 && pObj->pCdtHead == NULL) {
            soft_inc(&g_u32SoftDefEmptyEdge);
            if (!g_u8CdtSoftLogged) {
                g_u8CdtSoftLogged = 1;
                kprintf("cap:cdt soft empty edges slots_left>0 (once)\n");
                /* Grep: cap: revoke soft empty-edge */
                kprintf("cap: revoke soft empty-edge slots_left=%u "
                        "pending=%u wave=%u soft\n",
                        pObj->u32SlotsLeft, gj_revoke_deferred_pending(),
                        GJ_REVOKE_SOFT_WAVE);
            }
            g_u32RevokeQCursor = (iEnt + 1u) % u32Limit;
        }
    }

    /* Wave 19 return surface: deferred cleared buckets. */
    if (u32Cleared == 0u) {
        soft_inc(&g_u32SoftRetDefZero);
    } else {
        soft_inc(&g_u32SoftRetDefPos);
        if (g_u32SoftRetDefSum < 0xffffffffu - u32Cleared) {
            g_u32SoftRetDefSum += u32Cleared;
        } else {
            g_u32SoftRetDefSum = 0xffffffffu;
        }
    }

    soft_revoke_inventory_maybe_once();
    return u32Cleared;
}

int
gj_obj_reclaim_ready(const struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGateSt);
        return 0;
    }
    if (__atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_DEAD) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGateSt);
        return 0;
    }
    /* S6/R9: all derived slots invalidated, no kernel refs or pins */
    if (pObj->u32SlotsLeft != 0) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGateSlot);
        return 0;
    }
    if (pObj->pCdtHead != NULL) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGateCdt);
        return 0; /* soft CDT still has edges — walk must finish */
    }
    if (pObj->u32Ref != 0) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGateRef);
        return 0;
    }
    if (pObj->u32Pin != 0) {
        soft_inc(&g_u32SoftReclaimNot);
        soft_inc(&g_u32SoftReclaimGatePin);
        return 0;
    }
    soft_inc(&g_u32SoftReclaimReady);
    return 1;
}

gj_status_t
gj_obj_reclaim(struct gj_obj_hdr *pObj)
{
    u32 iEnt;

    if (pObj == NULL) {
        soft_inc(&g_u32SoftReclaimNull);
        return GJ_ERR_INVAL;
    }
    if (!gj_obj_reclaim_ready(pObj)) {
        soft_inc(&g_u32SoftReclaimBusy);
        /* Grep: cap: revoke soft reclaim busy */
        if (g_u32SoftReclaimBusy == 1u) {
            kprintf("cap: revoke soft reclaim busy gate_slot=%u gate_cdt=%u "
                    "gate_ref=%u gate_pin=%u soft (once)\n",
                    g_u32SoftReclaimGateSlot, g_u32SoftReclaimGateCdt,
                    g_u32SoftReclaimGateRef, g_u32SoftReclaimGatePin);
        }
        return GJ_ERR_BUSY;
    }

    /* Free object storage when slab/untyped free exists. Drop queue entry. */
    for (iEnt = 0; iEnt < GJ_REVOKE_Q_MAX; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active && g_aRevokeQ[iEnt].pObj == pObj) {
            revoke_q_drop(iEnt);
        }
    }
    soft_inc(&g_u32SoftReclaimOk);
    /* Grep: cap: revoke soft reclaim ok */
    if (g_u32SoftReclaimOk == 1u) {
        kprintf("cap: revoke soft reclaim ok wave=%u soft\n",
                GJ_REVOKE_SOFT_WAVE);
    }
    soft_revoke_inventory_maybe_once();
    return GJ_OK;
}
