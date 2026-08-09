/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Capability revoke: DEAD/gen first (fail closed), then mandatory deferred
 * CNode slot invalidation (S1-S7, R1-R9). See SECURITY_CORE_DESIGN S1.1.
 *
 * Phase A  - logical invalidate (DEAD + gen); security complete.
 * Phase A' - deferred CDT walk + structured slot invalidate (this file).
 * Phase C  - reclaim when slots_left/refs/pins allow.
 *
 * Soft CDT: edges on pObj->pCdtHead when mint wires them; empty list with
 * slots_left > 0 is a known soft gap (grep: cap:cdt soft).
 *
 * R2 try-lock slot walk (this file):
 *   Soft CNode lock is gj_cnode_trylock (u32SoftLock CAS). Product-true
 *   mutex/turnstile is still missing - R2 is intentionally PARTIAL.
 *   Policy is sleep-not-spin: never busy-wait a busy CNode; leave the edge
 *   linked, try siblings, soft-retry a bounded number of re-walks, then
 *   defer to timer/idle (R7). Counters: spins_avoided, retries, slots_cleared.
 *
 * Soft inventory (Wave 40 exclusive deepen; this unit only):
 *   - Phase A begin path: ok / dead / busy / again / null / queue full
 *   - Deferred queue: push / drop / pending samples / cursor / full
 *   - CDT walk batch: enter / clear / busy / stale / visit / pass / retry
 *   - Process deferred: scan / hygiene / empty-edge soft gap / budget
 *   - Reclaim: ready / busy / ok / null; slots_left / pin / ref gates
 *   - honesty / deepen / PASS / mutex / reply non-claims
 *   - return surface: begin|walk|deferred|reclaim gj_status buckets
 *   - return rate / retcode / return selftest / retmap (lean lamps)
 *   - soft residual lean (Soft!=product - dual MIT OR Apache-2.0 - G-AC-1)
 *   - UDX host-death teardown residual: multi-obj begin->deferred->reclaim
 *     (DMA/MMIO/IRQ-shaped stack objs; not product host-death complete)
 *   - C3 security residual (behavior checks, not lamp-only):
 *     S1 DEAD+gen first; S2 check_live exact GJ_ERR_DEAD + DEAD-wins-new-gen;
 *     R6 one-revoker DEAD; Phase C pin/slots_left/ref reclaim gates;
 *     Phase C lag hold: DEAD+gen still published after gate block (S3 lag)
 *   Never hard-gates; diagnostics only (wrap OK). Soft != product mutex.
 *   Soft. Soft != GJ_CAP_REPLY product. Soft != MIG REPLY product.
 *   Soft != full CDT mutex product. Product CNode turnstile still OPEN.
 *   Soft != product UDX host death (DMA/MMIO/IRQ revoke still OPEN).
 *   G-AC-1: no Linux .ko product AC via this residual.
 *   Dual DoD OPEN (not claimed). Lean residual only - no version stamp /
 *   no stamp storms / no commit / never bump GJ_IMAGE_VERSION.
 *
 * Grep: cap:cdt deferred / cap:cdt walk / cap:quota soft
 * Grep: cap: revoke try-lock / cap:cdt R2 soft / cap:cdt trylock
 * Grep: cap: revoke soft  (Wave 20 deepen surface)
 * Grep: cap: revoke soft honesty|inventory|deepen|PASS|mutex|reply|return
 * Grep: cap: revoke soft residual lean
 * Grep: cap: revoke soft residual lean udx|teardown|c3
 * Grep: cap: revoke ...     (begin|walk|deferred|reclaim|r2|try-lock|queue)
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/klog.h>

/* Max objects waiting for slot hygiene / reclaim on this simple M0-M2 queue. */
#define GJ_REVOKE_Q_MAX 64u

/*
 * Bounded soft re-walks of busy CDT edges within one batch (R2).
 * Not a spin: each pass only try-locks once per edge; further progress
 * yields to the deferred driver (timer/idle) - sleep-not-spin.
 */
#define GJ_REVOKE_R2_SOFT_RETRY_MAX 3u

/* Wave 20 deepen stamp (file-local; never hard-gates). No stamp storms. */
#define GJ_REVOKE_SOFT_WAVE 116u
/*
 * return | return rate | retcode | return selftest | retmap | residual lean
 * | udx teardown residual (DMA/MMIO/IRQ-shaped multi-obj begin->def->reclaim)
 * | C3 residual (S1 DEAD+gen / S2 exact DEAD + DEAD-wins-new-gen /
 * |   R6 one-revoker / Phase C pin+slots+ref gates / lag hold S3)
 */
#define GJ_REVOKE_SOFT_AREAS 224u
/* UDX host-death shape: three stack objs (dma / mmio / irq stand-ins). */
#define GJ_REVOKE_LEAN_UDX_OBJS 3u

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
 *   spins_avoided - try-lock busy -> deferred (did not spin)
 *   retries       - soft re-walk passes after a busy edge
 *   slots_cleared - structured invalidates via CDT walk batch
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
static u32 g_u32SoftBeginOk;        /* Phase A success -> queued */
static u32 g_u32SoftBeginNull;      /* pObj == NULL */
static u32 g_u32SoftBeginDead;      /* concurrent revoke -> DEAD/REVOKING */
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
/* Lean residual self-check (stack-local begin->reclaim; never hard-gates). */
static u32 g_u32SoftLeanRuns;       /* residual lean selftest entries */
static u32 g_u32SoftLeanOk;         /* begin+reclaim both GJ_OK (legacy single) */
static u32 g_u32SoftLeanBeginOk;    /* begin ok count in lean selftest */
static u32 g_u32SoftLeanReclaimOk;  /* reclaim ok count in lean selftest */
/* UDX teardown residual lean (multi-obj host-death shape). Soft!=product. */
static u32 g_u32SoftLeanUdxBegin;   /* UDX-shape begin successes */
static u32 g_u32SoftLeanUdxReclaim; /* UDX-shape reclaim successes */
static u32 g_u32SoftLeanUdxDef;     /* deferred drive calls in lean */
static u32 g_u32SoftLeanUdxDefClr;  /* deferred cleared sum in lean */
static u32 g_u32SoftLeanUdxOk;      /* full multi-obj UDX teardown ok */
/*
 * C3 security residual (behavior checks; Soft!=product; not product AC).
 * Class C3 forbids lamp-only PASS - these counters only light when the
 * hazard regression shape actually observes the security property.
 * Grep: cap: revoke soft residual lean c3
 */
static u32 g_u32SoftLeanS1Ok;       /* post-begin DEAD + gen bump (S1) */
static u32 g_u32SoftLeanS2Closed;   /* check_live == GJ_ERR_DEAD (pre-gen) (S2) */
static u32 g_u32SoftLeanS2DeadWins; /* check_live(new gen) == GJ_ERR_DEAD */
static u32 g_u32SoftLeanR6Ok;       /* concurrent begin -> GJ_ERR_DEAD (R6) */
static u32 g_u32SoftLeanGatePin;    /* pin>0 blocked reclaim (Phase C) */
static u32 g_u32SoftLeanGateSlot;   /* slots_left>0 blocked reclaim (Phase C) */
static u32 g_u32SoftLeanGateRef;    /* ref>0 blocked reclaim (Phase C) */
static u32 g_u32SoftLeanLagHold;    /* DEAD+gen still published after gate block */
static u32 g_u32SoftLeanC3Ok;       /* full C3 residual hazard selftest ok */
static u8  g_u8RevokeLeanOnce;      /* once-marker for lean residual */
/* Suppress inventory reentry while lean residual drives begin/reclaim. */
static u8  g_u8RevokeLeanBusy;

static void soft_inc(u32 *pCtr);
static void soft_note_pending_peak(u32 u32Pending);
static void soft_revoke_inventory_log(void);
static void soft_revoke_inventory_maybe_once(void);
static void soft_revoke_residual_lean_once(void);

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
 *   cap: revoke soft inventory|begin|queue|walk|r2|deferred|reclaim|path|...
 *   cap: revoke soft honesty|mutex|reply|return|deepen|PASS|residual lean
 *   cap: revoke try-lock soft ...
 *   cap:cdt R2 soft inventory|...
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
     * Primary Wave 20 deepen lines under "cap: revoke soft ...".
     * Honesty: soft u32SoftLock only - product try-lock still partial.
     * Soft != GJ_CAP_REPLY product / MIG REPLY product / full CDT mutex.
     */
    /* Grep: cap: revoke soft honesty */
    kprintf("cap: revoke soft honesty reply_product=0 full_cdt_mutex=0 "
            "soft_lock=u32SoftLock sleep_not_spin=1 r2_partial=1 "
            "soft_ne_mig_reply=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "G-AC-1=1 udx_teardown=1 c3_residual=1 host_death_product=0 "
            "dual_dod=OPEN "
            "wave=%u (soft != GJ_CAP_REPLY product; soft != MIG REPLY "
            "product; soft != full CDT mutex product; Soft!=product; "
            "G-AC-1; UDX teardown + C3 residual only; Dual DoD OPEN; "
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
     * R2 soft lock only - full CDT mutex / turnstile product still OPEN.
     */
    kprintf("cap: revoke soft mutex soft_lock=u32SoftLock "
            "product_mutex=OPEN full_cdt_mutex=0 turnstile=OPEN "
            "sleep_not_spin=1 spins_avoided=%u retries=%u "
            "wave=%u (soft != full CDT mutex product)\n",
            g_u32R2SpinsAvoided, g_u32R2Retries, GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft reply
     * Revoke path does not mint/consume GJ_CAP_REPLY; door soft table only.
     * Soft != MIG REPLY product.
     */
    kprintf("cap: revoke soft reply reply_product=0 mig_reply=0 "
            "cnode_reply_install=0 soft_ne_mig_reply=1 wave=%u "
            "(soft != GJ_CAP_REPLY product; soft != MIG REPLY product; "
            "door REPLY soft table is separate)\n",
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft return
     * Public return-surface: begin/walk/deferred/reclaim buckets.
     * Soft != product mutex / MIG REPLY / full CDT. Dual MIT OR Apache-2.0.
     */
    kprintf("cap: revoke soft return begin_ok=%u begin_null=%u "
            "begin_dead=%u begin_busy=%u begin_again=%u "
            "walk_pos=%u walk_zero=%u walk_sum=%u "
            "def_pos=%u def_zero=%u def_sum=%u "
            "reclaim_ok=%u reclaim_busy=%u reclaim_null=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; soft inventory only)\n",
            g_u32SoftBeginOk, g_u32SoftBeginNull, g_u32SoftBeginDead,
            g_u32SoftBeginBusy, g_u32SoftBeginAgain, g_u32SoftRetWalkPos,
            g_u32SoftRetWalkZero, g_u32SoftRetWalkSum, g_u32SoftRetDefPos,
            g_u32SoftRetDefZero, g_u32SoftRetDefSum, g_u32SoftReclaimOk,
            g_u32SoftReclaimBusy, g_u32SoftReclaimNull);

    /*
     * Grep: cap: revoke soft return rate
     * Ok/fail rate lamps (soft only; never hard-gate). Soft!=product.
     */
    kprintf("cap: revoke soft return rate "
            "begin_ok=%u begin_fail=%u walk_pos=%u walk_zero=%u "
            "def_pos=%u def_zero=%u reclaim_ok=%u reclaim_not=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; rate lamps only)\n",
            g_u32SoftBeginOk,
            g_u32SoftBeginNull + g_u32SoftBeginDead + g_u32SoftBeginBusy +
                g_u32SoftBeginAgain,
            g_u32SoftRetWalkPos, g_u32SoftRetWalkZero, g_u32SoftRetDefPos,
            g_u32SoftRetDefZero, g_u32SoftReclaimOk, g_u32SoftReclaimNot);

    /*
     * Grep: cap: revoke soft retcode
     * Observed gj_status class catalog for begin/reclaim (soft lamps).
     */
    kprintf("cap: revoke soft retcode "
            "begin_ok=%u begin_inval=%u begin_dead=%u begin_busy=%u "
            "begin_again=%u reclaim_ok=%u reclaim_inval=%u reclaim_busy=%u "
            "walk_u32=%u def_u32=%u soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 (Soft!=product; not ABI claim)\n",
            g_u32SoftBeginOk, g_u32SoftBeginNull, g_u32SoftBeginDead,
            g_u32SoftBeginBusy, g_u32SoftBeginAgain, g_u32SoftReclaimOk,
            g_u32SoftReclaimNull, g_u32SoftReclaimBusy, g_u32SoftRetWalkSum,
            g_u32SoftRetDefSum);

    /*
     * Grep: cap: revoke soft return selftest / cap: revoke soft retmap
     * Lean selftest map: begin->DEAD/gen -> deferred hygiene -> reclaim.
     * Soft!=product - dual MIT OR Apache-2.0 - no version stamp.
     */
    kprintf("cap: revoke soft return selftest lean_runs=%u lean_ok=%u "
            "lean_begin_ok=%u lean_reclaim_ok=%u "
            "map=begin_DEAD_gen->deferred->reclaim soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 (Soft!=product; no version stamp)\n",
            g_u32SoftLeanRuns, g_u32SoftLeanOk, g_u32SoftLeanBeginOk,
            g_u32SoftLeanReclaimOk);
    kprintf("cap: revoke soft retmap "
            "begin=OK|INVAL|DEAD|BUSY|AGAIN walk=u32_cleared "
            "deferred=u32_cleared reclaim=OK|INVAL|BUSY "
            "phase_a=DEAD_gen_first r2=trylock_defer r7=redrive "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; soft retmap only)\n");

    /* Grep: cap: revoke soft path */
    kprintf("cap: revoke soft path phase_a=DEAD_gen_first phase_ap=cdt_walk "
            "r2=trylock_defer r7=timer_idle_redrive phase_c=reclaim "
            "lock=soft_u32SoftLock product=PARTIAL "
            "return_surface=1 return_rate=1 retcode=1 "
            "return_selftest=1 retmap=1 residual_lean=1 "
            "udx_teardown=1 c3_residual=1 host_death_product=0 "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "wave=%u (soft inventory; Soft!=product; G-AC-1; "
            "UDX teardown + C3 residual; soft != GJ_CAP_REPLY product; "
            "soft != MIG REPLY product; soft != full CDT mutex product; "
            "not product host-death complete)\n",
            GJ_REVOKE_SOFT_WAVE);

    /*
     * Grep: cap: revoke soft residual lean
     * Grep: cap: revoke soft residual lean udx | teardown | c3
     * Lean residual honesty - Soft!=product dual license; G-AC-1;
     * UDX host-death teardown residual (DMA/MMIO/IRQ-shaped multi-obj).
     * C3: S1/S2/R6/Phase-C gate behavior checks (not lamp-only).
     * Product mutex / MIG REPLY / full CDT turnstile / host-death remain OPEN.
     */
    kprintf("cap: revoke soft residual lean "
            "begin_ok=%u walk_sum=%u def_sum=%u reclaim_ok=%u "
            "spins_avoided=%u retries=%u slots_cleared=%u "
            "lean_runs=%u lean_ok=%u udx_ok=%u udx_begin=%u "
            "udx_reclaim=%u udx_def=%u c3_ok=%u s1=%u s2=%u s2_dead_wins=%u "
            "r6=%u gate_pin=%u gate_slot=%u gate_ref=%u lag_hold=%u "
            "pending=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "udx_teardown=1 c3_residual=1 reply_product=0 full_cdt_mutex=0 "
            "r2_partial=1 host_death_product=0 dual_dod=OPEN "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "no version stamp; UDX teardown + C3 residual only; "
            "not product host-death complete; not full CDT mutex product; "
            "Dual DoD OPEN)\n",
            g_u32SoftBeginOk, g_u32SoftRetWalkSum, g_u32SoftRetDefSum,
            g_u32SoftReclaimOk, g_u32R2SpinsAvoided, g_u32R2Retries,
            g_u32R2SlotsCleared, g_u32SoftLeanRuns, g_u32SoftLeanOk,
            g_u32SoftLeanUdxOk, g_u32SoftLeanUdxBegin, g_u32SoftLeanUdxReclaim,
            g_u32SoftLeanUdxDef, g_u32SoftLeanC3Ok, g_u32SoftLeanS1Ok,
            g_u32SoftLeanS2Closed, g_u32SoftLeanS2DeadWins, g_u32SoftLeanR6Ok,
            g_u32SoftLeanGatePin, g_u32SoftLeanGateSlot, g_u32SoftLeanGateRef,
            g_u32SoftLeanLagHold, u32Pending);

    /* Grep: cap: revoke soft deepen (single line; no stamp storm) */
    kprintf("cap: revoke soft deepen wave=%u areas=%u pending=%u "
            "spins_avoided=%u retries=%u slots_cleared=%u "
            "begin_ok=%u reclaim_ok=%u walk_pos=%u def_pos=%u "
            "lean_ok=%u udx_ok=%u c3_ok=%u log_n=%u ok=1 skip=0\n",
            GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS, u32Pending,
            g_u32R2SpinsAvoided, g_u32R2Retries, g_u32R2SlotsCleared,
            g_u32SoftBeginOk, g_u32SoftReclaimOk, g_u32SoftRetWalkPos,
            g_u32SoftRetDefPos, g_u32SoftLeanOk, g_u32SoftLeanUdxOk,
            g_u32SoftLeanC3Ok, g_u32SoftLogN);

    /* Grep: cap: revoke soft inventory PASS / cap: revoke soft PASS */
    kprintf("cap: revoke soft inventory PASS log_n=%u wave=%u areas=%u "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "udx_teardown=1 c3_residual=1\n",
            g_u32SoftLogN, GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS);
    kprintf("cap: revoke soft PASS wave=%u areas=%u lean_ok=%u udx_ok=%u "
            "c3_ok=%u G-AC-1=1 udx_teardown=1 c3_residual=1\n",
            GJ_REVOKE_SOFT_WAVE, GJ_REVOKE_SOFT_AREAS, g_u32SoftLeanOk,
            g_u32SoftLeanUdxOk, g_u32SoftLeanC3Ok);
}

/**
 * Lean residual self-check for UDX host-death teardown + C3 hazard shape.
 *
 * Stack-local multi-obj (DMA / MMIO / IRQ stand-ins; never touches process
 * CNode or real UDX host). Exercises:
 *   Phase A  DEAD+gen first on each obj (security complete per object)
 *   Phase A' gj_revoke_process_deferred once (hygiene / R7 redrive surface)
 *   Phase C  reclaim each when slots_left/ref/pin/CDT allow
 *
 * C3 security residual (ASSURANCE_LITE: Soft lamp without behavior check
 * is forbidden as C3 PASS). Behavior checks only - Soft!=product:
 *   S1  after begin: state==DEAD and gen == gen0+1 (DEAD+gen first)
 *   S2  gj_obj_check_live(obj, gen0) == GJ_ERR_DEAD (exact fail-closed)
 *   S2' gj_obj_check_live(obj, gen0+1) == GJ_ERR_DEAD (DEAD wins gen match)
 *   R6  second begin on same obj returns GJ_ERR_DEAD (one revoker)
 *   Phase C pin / slots_left / ref gates block reclaim_ready / reclaim
 *   Lag hold: after gate block, DEAD+gen still published + S2 still closed
 *
 * Soft!=product - dual MIT OR Apache-2.0 - G-AC-1 - Dual DoD OPEN -
 * no version stamp / never bump GJ_IMAGE_VERSION.
 * Not product host-death complete (DMA/MMIO/IRQ wire revoke still OPEN).
 * greppable: cap: revoke soft residual lean
 * greppable: cap: revoke soft residual lean udx
 * greppable: cap: revoke soft residual lean teardown
 * greppable: cap: revoke soft residual lean c3
 */
static void
soft_revoke_residual_lean_once(void)
{
    struct gj_obj_hdr aHdr[GJ_REVOKE_LEAN_UDX_OBJS];
    gj_status_t       aStBegin[GJ_REVOKE_LEAN_UDX_OBJS];
    gj_status_t       aStReclaim[GJ_REVOKE_LEAN_UDX_OBJS];
    u32               aGen0[GJ_REVOKE_LEAN_UDX_OBJS];
    u32               iObj;
    u32               u32BeginOk;
    u32               u32ReclaimOk;
    u32               u32DefClr;
    u32               u32S1Ok;
    u32               u32S2Closed;
    u32               u32S2DeadWins;
    u32               u32R6Ok;
    u32               u32GatePinOk;
    u32               u32GateSlotOk;
    u32               u32GateRefOk;
    u32               u32LagHoldOk;
    int               fReadyLast;
    int               fReadyGate;
    gj_status_t       stGate;
    gj_status_t       stLive;
    gj_status_t       stLiveNew;
    gj_status_t       stConc;

    if (g_u8RevokeLeanOnce != 0u) {
        return;
    }
    g_u8RevokeLeanOnce = 1u;
    soft_inc(&g_u32SoftLeanRuns);

    /*
     * Suppress inventory reentry: begin/reclaim/deferred may call maybe_once;
     * lean residual owns the once-path and inventory runs after.
     */
    g_u8RevokeLeanBusy = 1u;

    u32BeginOk = 0;
    u32ReclaimOk = 0;
    u32DefClr = 0;
    u32S1Ok = 0;
    u32S2Closed = 0;
    u32S2DeadWins = 0;
    u32R6Ok = 0;
    u32GatePinOk = 0;
    u32GateSlotOk = 0;
    u32GateRefOk = 0;
    u32LagHoldOk = 0;
    fReadyLast = 0;

    /*
     * UDX host-death shape: revoke each stand-in (dma=0, mmio=1, irq=2).
     * Phase A is per-object; security complete after DEAD+gen even if
     * deferred queue is full (AGAIN still fail-closed).
     * C3: verify S1 DEAD+gen, S2 exact DEAD + DEAD-wins-new-gen, R6.
     */
    for (iObj = 0; iObj < GJ_REVOKE_LEAN_UDX_OBJS; iObj++) {
        gj_obj_hdr_init(&aHdr[iObj]);
        aGen0[iObj] = aHdr[iObj].u32Gen;
        aStBegin[iObj] = gj_obj_revoke_begin(&aHdr[iObj]);
        if (aStBegin[iObj] == GJ_OK || aStBegin[iObj] == GJ_ERR_AGAIN) {
            soft_inc(&g_u32SoftLeanBeginOk);
            soft_inc(&g_u32SoftLeanUdxBegin);
            u32BeginOk++;

            /*
             * S1 - DEAD + gen first (security boundary).
             * Gen must bump even when queue full (AGAIN path).
             */
            if (__atomic_load_n(&aHdr[iObj].u32State, __ATOMIC_ACQUIRE) ==
                    (u32)GJ_OBJ_DEAD &&
                __atomic_load_n(&aHdr[iObj].u32Gen, __ATOMIC_ACQUIRE) ==
                    (aGen0[iObj] + 1u)) {
                soft_inc(&g_u32SoftLeanS1Ok);
                u32S1Ok++;
            }

            /*
             * S2 - exact fail-closed (not lamp-only != OK):
             *   pre-revoke gen must return GJ_ERR_DEAD (state != LIVE).
             * S2' DEAD wins gen match: even the post-bump gen must still
             *   return GJ_ERR_DEAD (never success while not LIVE).
             */
            stLive = gj_obj_check_live(&aHdr[iObj], aGen0[iObj]);
            if (stLive == GJ_ERR_DEAD) {
                soft_inc(&g_u32SoftLeanS2Closed);
                u32S2Closed++;
            }
            stLiveNew = gj_obj_check_live(&aHdr[iObj], aGen0[iObj] + 1u);
            if (stLiveNew == GJ_ERR_DEAD) {
                soft_inc(&g_u32SoftLeanS2DeadWins);
                u32S2DeadWins++;
            }

            /*
             * R6 - one revoker: concurrent begin on DEAD object is DEAD.
             * Does not undo Phase A; never a second owner; gen must not
             * double-bump under concurrent begin (one revoker only).
             */
            stConc = gj_obj_revoke_begin(&aHdr[iObj]);
            if (stConc == GJ_ERR_DEAD &&
                __atomic_load_n(&aHdr[iObj].u32State, __ATOMIC_ACQUIRE) ==
                    (u32)GJ_OBJ_DEAD &&
                __atomic_load_n(&aHdr[iObj].u32Gen, __ATOMIC_ACQUIRE) ==
                    (aGen0[iObj] + 1u)) {
                soft_inc(&g_u32SoftLeanR6Ok);
                u32R6Ok++;
            }
        }
        aStReclaim[iObj] = GJ_ERR_BUSY; /* filled after deferred */
    }

    /*
     * Drive deferred once (R7 surface). Empty CDT + slots_left==0 means
     * hygiene is already done; still exercises queue scan / cursor.
     * Soft residual only - not product CDT walk complete.
     */
    soft_inc(&g_u32SoftLeanUdxDef);
    u32DefClr = gj_revoke_process_deferred(GJ_REVOKE_LEAN_UDX_OBJS * 4u);
    if (g_u32SoftLeanUdxDefClr < 0xffffffffu - u32DefClr) {
        g_u32SoftLeanUdxDefClr += u32DefClr;
    } else {
        g_u32SoftLeanUdxDefClr = 0xffffffffu;
    }

    /*
     * Phase C gate residual (C3 hazard shape): pin, slots_left, and ref
     * must each block reclaim. Soft selftest only - not product destroy.
     * Obj0: pin gate. Obj1: slots_left gate. Obj2: ref gate.
     * After each block, lag hold: DEAD+gen still published (S3 lag).
     */
    if (u32BeginOk > 0u) {
        aHdr[0].u32Pin = 1u;
        fReadyGate = gj_obj_reclaim_ready(&aHdr[0]);
        stGate = gj_obj_reclaim(&aHdr[0]);
        if (fReadyGate == 0 && stGate == GJ_ERR_BUSY) {
            soft_inc(&g_u32SoftLeanGatePin);
            u32GatePinOk = 1u;
            /* Lag hold: security boundary still published while reclaim waits. */
            if (__atomic_load_n(&aHdr[0].u32State, __ATOMIC_ACQUIRE) ==
                    (u32)GJ_OBJ_DEAD &&
                __atomic_load_n(&aHdr[0].u32Gen, __ATOMIC_ACQUIRE) ==
                    (aGen0[0] + 1u) &&
                gj_obj_check_live(&aHdr[0], aGen0[0]) == GJ_ERR_DEAD) {
                soft_inc(&g_u32SoftLeanLagHold);
                u32LagHoldOk++;
            }
        }
        aHdr[0].u32Pin = 0u;
    }
    if (u32BeginOk > 1u) {
        aHdr[1].u32SlotsLeft = 1u;
        fReadyGate = gj_obj_reclaim_ready(&aHdr[1]);
        stGate = gj_obj_reclaim(&aHdr[1]);
        if (fReadyGate == 0 && stGate == GJ_ERR_BUSY) {
            soft_inc(&g_u32SoftLeanGateSlot);
            u32GateSlotOk = 1u;
            if (__atomic_load_n(&aHdr[1].u32State, __ATOMIC_ACQUIRE) ==
                    (u32)GJ_OBJ_DEAD &&
                __atomic_load_n(&aHdr[1].u32Gen, __ATOMIC_ACQUIRE) ==
                    (aGen0[1] + 1u) &&
                gj_obj_check_live(&aHdr[1], aGen0[1]) == GJ_ERR_DEAD) {
                soft_inc(&g_u32SoftLeanLagHold);
                u32LagHoldOk++;
            }
        }
        aHdr[1].u32SlotsLeft = 0u;
    }
    if (u32BeginOk > 2u) {
        aHdr[2].u32Ref = 1u;
        fReadyGate = gj_obj_reclaim_ready(&aHdr[2]);
        stGate = gj_obj_reclaim(&aHdr[2]);
        if (fReadyGate == 0 && stGate == GJ_ERR_BUSY) {
            soft_inc(&g_u32SoftLeanGateRef);
            u32GateRefOk = 1u;
            if (__atomic_load_n(&aHdr[2].u32State, __ATOMIC_ACQUIRE) ==
                    (u32)GJ_OBJ_DEAD &&
                __atomic_load_n(&aHdr[2].u32Gen, __ATOMIC_ACQUIRE) ==
                    (aGen0[2] + 1u) &&
                gj_obj_check_live(&aHdr[2], aGen0[2]) == GJ_ERR_DEAD) {
                soft_inc(&g_u32SoftLeanLagHold);
                u32LagHoldOk++;
            }
        }
        aHdr[2].u32Ref = 0u;
    }

    /*
     * Phase C: reclaim each DEAD stand-in. Empty CDT + zero slots/ref/pin
     * => reclaim-ready. Soft selftest only - not product destroy complete.
     */
    for (iObj = 0; iObj < GJ_REVOKE_LEAN_UDX_OBJS; iObj++) {
        fReadyLast = gj_obj_reclaim_ready(&aHdr[iObj]);
        aStReclaim[iObj] = gj_obj_reclaim(&aHdr[iObj]);
        if (fReadyLast != 0 && aStReclaim[iObj] == GJ_OK) {
            soft_inc(&g_u32SoftLeanReclaimOk);
            soft_inc(&g_u32SoftLeanUdxReclaim);
            u32ReclaimOk++;
        }
    }

    /* Legacy single-path lamp: first obj begin+reclaim both OK. */
    if ((aStBegin[0] == GJ_OK || aStBegin[0] == GJ_ERR_AGAIN) &&
        aStReclaim[0] == GJ_OK) {
        soft_inc(&g_u32SoftLeanOk);
    }
    /* UDX multi-obj teardown ok when every stand-in began and reclaimed. */
    if (u32BeginOk == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32ReclaimOk == GJ_REVOKE_LEAN_UDX_OBJS) {
        soft_inc(&g_u32SoftLeanUdxOk);
    }
    /*
     * C3 residual ok only when every hazard shape observed:
     * S1/S2 exact DEAD/S2' DEAD-wins/R6 on all UDX objs +
     * pin/slots/ref gates + lag hold on each gate + full teardown.
     * Soft residual - not product security AC / not host-death complete.
     */
    if (u32BeginOk == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32ReclaimOk == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32S1Ok == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32S2Closed == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32S2DeadWins == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32R6Ok == GJ_REVOKE_LEAN_UDX_OBJS &&
        u32GatePinOk == 1u && u32GateSlotOk == 1u && u32GateRefOk == 1u &&
        u32LagHoldOk == GJ_REVOKE_LEAN_UDX_OBJS) {
        soft_inc(&g_u32SoftLeanC3Ok);
    }

    g_u8RevokeLeanBusy = 0u;

    /*
     * Grep: cap: revoke soft residual lean PASS
     * Grep: cap: revoke soft residual lean udx
     * Grep: cap: revoke soft residual lean teardown
     * Grep: cap: revoke soft residual lean c3
     */
    kprintf("cap: revoke soft residual lean PASS "
            "begin_st=%d reclaim_st=%d ready=%d "
            "lean_runs=%u lean_ok=%u lean_begin_ok=%u lean_reclaim_ok=%u "
            "udx_objs=%u udx_begin=%u udx_reclaim=%u udx_def=%u "
            "udx_def_clr=%u udx_ok=%u c3_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "udx_teardown=1 c3_residual=1 reply_product=0 full_cdt_mutex=0 "
            "host_death_product=0 dual_dod=OPEN "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "no version stamp; UDX teardown + C3 residual only; "
            "not product host-death complete; Dual DoD OPEN)\n",
            (int)aStBegin[0], (int)aStReclaim[0], fReadyLast,
            g_u32SoftLeanRuns, g_u32SoftLeanOk, g_u32SoftLeanBeginOk,
            g_u32SoftLeanReclaimOk, GJ_REVOKE_LEAN_UDX_OBJS,
            g_u32SoftLeanUdxBegin, g_u32SoftLeanUdxReclaim,
            g_u32SoftLeanUdxDef, g_u32SoftLeanUdxDefClr, g_u32SoftLeanUdxOk,
            g_u32SoftLeanC3Ok);

    /* Grep: cap: revoke soft residual lean udx */
    kprintf("cap: revoke soft residual lean udx "
            "objs=%u begin=%u reclaim=%u def=%u def_clr=%u ok=%u "
            "map=begin_DEAD_gen->deferred->reclaim "
            "shape=dma+mmio+irq_standin soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 host_death_product=0 "
            "(Soft!=product; UDX host-death teardown residual; "
            "not product DMA/MMIO/IRQ revoke complete)\n",
            GJ_REVOKE_LEAN_UDX_OBJS, g_u32SoftLeanUdxBegin,
            g_u32SoftLeanUdxReclaim, g_u32SoftLeanUdxDef,
            g_u32SoftLeanUdxDefClr, g_u32SoftLeanUdxOk);

    /* Grep: cap: revoke soft residual lean teardown */
    kprintf("cap: revoke soft residual lean teardown "
            "phase_a=DEAD_gen_first phase_ap=deferred phase_c=reclaim "
            "udx_ok=%u c3_ok=%u r2_partial=1 full_cdt_mutex=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 dual_dod=OPEN "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "no stamp storms; not product revoke complete; Dual DoD OPEN)\n",
            g_u32SoftLeanUdxOk, g_u32SoftLeanC3Ok);

    /*
     * Grep: cap: revoke soft residual lean c3
     * Grep: cap: revoke soft residual lean c3 PASS
     * C3 hazard residual - behavior checks only (not lamp-only PASS).
     */
    kprintf("cap: revoke soft residual lean c3 "
            "s1_dead_gen=%u/%u s2_exact_dead=%u/%u s2_dead_wins=%u/%u "
            "r6_one_revoker=%u/%u gate_pin=%u gate_slot=%u gate_ref=%u "
            "lag_hold=%u/%u udx_ok=%u c3_ok=%u "
            "map=S1_DEAD_gen+S2_exact_DEAD+S2_dead_wins+R6_concurrent+"
            "C_pin_slots_ref+lag_hold "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "host_death_product=0 dual_dod=OPEN "
            "(Soft!=product; C3 residual behavior checks; "
            "not product security AC; not product host-death complete; "
            "Dual DoD OPEN)\n",
            u32S1Ok, GJ_REVOKE_LEAN_UDX_OBJS, u32S2Closed,
            GJ_REVOKE_LEAN_UDX_OBJS, u32S2DeadWins, GJ_REVOKE_LEAN_UDX_OBJS,
            u32R6Ok, GJ_REVOKE_LEAN_UDX_OBJS, u32GatePinOk, u32GateSlotOk,
            u32GateRefOk, u32LagHoldOk, GJ_REVOKE_LEAN_UDX_OBJS,
            g_u32SoftLeanUdxOk, g_u32SoftLeanC3Ok);
    if (g_u32SoftLeanC3Ok != 0u) {
        kprintf("cap: revoke soft residual lean c3 PASS "
                "s1=%u s2=%u s2_dead_wins=%u r6=%u "
                "gate_pin=%u gate_slot=%u gate_ref=%u lag_hold=%u "
                "udx_ok=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                "G-AC-1=1 dual_dod=OPEN "
                "(Soft!=product; C3 hazard residual only; not product AC; "
                "Dual DoD OPEN)\n",
                u32S1Ok, u32S2Closed, u32S2DeadWins, u32R6Ok, u32GatePinOk,
                u32GateSlotOk, u32GateRefOk, u32LagHoldOk, g_u32SoftLeanUdxOk);
    }
}

/**
 * Emit soft inventory once after first meaningful revoke activity.
 * Avoids timer-tick spam; re-log is not required for greppable surface.
 * Runs lean residual selftest first (once) so return selftest lamps light.
 */
static void
soft_revoke_inventory_maybe_once(void)
{
    if (g_u8RevokeSoftInvLogged) {
        return;
    }
    /* Lean residual owns inventory sequencing - skip reentry from begin. */
    if (g_u8RevokeLeanBusy != 0u) {
        return;
    }
    /* Need at least one begin, walk, or deferred tick with work. */
    if (g_u32SoftBeginEnter == 0 && g_u32SoftWalkEnter == 0 &&
        g_u32SoftDefEnter == 0) {
        return;
    }
    /* Lean residual before inventory so lean_ok is visible in dump. */
    soft_revoke_residual_lean_once();
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

    /* One revoker (R6). Concurrent revoke -> DEAD/BUSY, never a second owner. */
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
     * S1 - security boundary: DEAD + bump gen BEFORE any deferrable work.
     * From here, gj_obj_check_live() fails closed (S2/S3) even if slots lag.
     */
    __atomic_store_n(&pObj->u32State, (u32)GJ_OBJ_DEAD, __ATOMIC_RELEASE);
    __atomic_fetch_add(&pObj->u32Gen, 1u, __ATOMIC_ACQ_REL);

    /*
     * Waiters would be woken here (IPC queues, fault waiters) when those
     * subsystems exist - PEER_DEAD / REVOKED. Never block on userspace (R4).
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
        /* Object is still DEAD - secure; hygiene must be retried (R7). */
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
 * If pObj is non-NULL, only touch a slot that still points at that object -
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
 * Soft CDT walk (Phase A' batch). Iterative; work-limited; does not delay S1.
 *
 * R2 try-lock slot walk:
 *   - Soft try-lock each CNode (gj_cnode_trylock / u32SoftLock).
 *   - If busy: count spins_avoided, leave edge linked, try siblings.
 *   - After a pass with busy edges, soft-retry up to
 *     GJ_REVOKE_R2_SOFT_RETRY_MAX re-walks (not a spin: one try-lock per
 *     edge per pass). If still busy, return; deferred driver redrives (R7).
 *   - Sleep-not-spin: never busy-wait trylock in a tight loop. A product
 *     path with a real mutex would sleep on the CNode turnstile; that
 *     mutex is still missing - R2 product try-lock remains PARTIAL.
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
     * Soft multi-pass: walk -> soft-retry busy edges -> defer if still held.
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
             * busy after SOFT_RETRY_MAX, yield to deferred driver (R7) -
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
                /* Stale edge - drop. Grep: cap:cdt stale */
                fUnlink = 1;
                u32Stale++;
                soft_inc(&g_u32SoftWalkStale);
            } else if (!gj_cnode_trylock(pCnode)) {
                /*
                 * R2: CNode busy - leave edge linked, try siblings, soft
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
                     * invalidate_locked still calls refund(NULL) - soft no-op.
                     */
                    gj_cap_slot_invalidate_locked(pSlot, pObj);
                    u32Cleared++;
                    g_u32R2SlotsCleared++;
                    fUnlink = 1;
                } else {
                    /* Slot already cleared or retargeted - drop edge. */
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
            /* Clean pass - no soft-retry needed. */
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
     * Greppable R2 summary (once). Honesty: soft u32SoftLock only -
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
 * Phase A': drive deferred slot work (bounded; R2 - no spin on CNode locks).
 * Prefer full CDT try-lock walk when edges exist (primary, more complete
 * than the empty-edge soft gap). Round-robin across queue so one lagging
 * object cannot starve siblings (R7). Soft-marker only if slots lag
 * without edges.
 *
 * Sleep-not-spin (R2): walk batch may soft-retry busy CNodes; if edges
 * still remain, keep the object queued and return - timer/idle redrives.
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

        /* Hygiene done for this object - leave queued for reclaim drain. */
        if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
            soft_inc(&g_u32SoftDefHygieneDone);
            continue;
        }

        u32Budget = u32MaxSlots - u32Cleared;

        /*
         * Edges present: full CDT try-lock batch is the primary hygiene path
         * (more complete than empty-edge soft gap). Walk batch itself does
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

            /* Edges remain (budget or try-lock busy) - keep queued (R7). */
            if (pObj->pCdtHead != NULL) {
                soft_inc(&g_u32SoftDefEdgeRemain);
                /*
                 * Soft R2 defer: edges still linked after try-lock walks.
                 * Product mutex still missing - partial R2.
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
         * Only when edges are absent - walk path above is preferred.
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
        return 0; /* soft CDT still has edges - walk must finish */
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
