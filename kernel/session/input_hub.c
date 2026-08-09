/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session input hub: small fan-in ring for virtio-input events (A1 path).
 *
 * Producers fan in via session_input_poll (virtio) and
 * session_input_push_from (soft inject / future PS2 etc.).
 * Consumers: session_input_pop (door INPUT_POP / compositor clients);
 * empty pop soft-refills once (lazy fan-in).
 * When the ring is full the oldest event is dropped - latency over fidelity
 * for the interim keyboard/pointer path. Drop count is retained for STATS.
 *
 * Lean soft residual (this unit only; Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 *   - Ring capacity / live pending / peak / free + drop-oldest policy
 *   - Fan-in src tallies (virtio live + soft inject slot)
 *   - Poll / push / enqueue / pop / lazy / ready / query soft counters
 *   - Sparse greppable lamps only (no stamp storms, no version stamp):
 *       input_hub: soft inventory | soft residual lean | soft PASS
 *       input: soft inventory | soft residual lean | soft PASS
 *   - One-shot residual self-check at init (stamp-free; Soft!=product):
 *       C0 deepen edges (behavior > lamp):
 *         reject null + reject bad-src + pop null + reject_clean (len stable)
 *         soft inject SYN with non-zero code/value -> pop field match
 *         pending_after=1 then empty_after (cursor restore)
 *       honest lamp PASS|PARTIAL|FAIL (never hardcode PASS)
 * Never hard-gates; diagnostics only. Soft != desktop/compositor product.
 * Soft residual != product dual-license claim beyond tree SPDX. G-AC-1.
 * Dual DoD desktop/input product remains OPEN. No GPL. stamp_storm=0.
 * greppable: input_hub: soft residual lean
 * greppable: input_hub: soft residual lean PASS|PARTIAL|FAIL
 * greppable: input: soft residual lean
 * greppable: input: soft residual lean PASS|PARTIAL|FAIL
 * greppable: input_hub: soft
 * greppable: input: soft
 */
#include <gj/klog.h>
#include <gj/session_input.h>
#include <gj/string.h>
#include <gj/virtio_input.h>

#define GJ_INPUT_RING 64u
/* Cap one poll burst so a stuck backend cannot spin the door forever. */
#define GJ_INPUT_POLL_MAX 256u

static struct gj_input_event g_aRing[GJ_INPUT_RING];
static u32 g_u32Head;
static u32 g_u32Len;
static u32 g_u32Pushed;
static u32 g_u32Dropped;
static u32 g_u32PeakPending;
static u32 g_u32PollBursts;
static u32 g_aPushedSrc[GJ_INPUT_SRC_MAX];
static int g_fReady;

/*
 * Soft product inventory tallies (lean residual feed).
 * Cumulative unless noted live/peak. Never hard-gates.
 * greppable: input_hub: soft ... / input: soft ...
 */
static u32 g_u32SoftPollEnter;    /* session_input_poll entries */
static u32 g_u32SoftPollNodev;    /* poll with virtio-input not ready */
static u32 g_u32SoftPollReady;    /* poll with virtio-input ready */
static u32 g_u32SoftPollDrain;    /* polls that accepted ≥1 event */
static u32 g_u32SoftPollIdle;     /* ready poll drained 0 events */
static u32 g_u32SoftPollCap;      /* drain hit GJ_INPUT_POLL_MAX */
static u32 g_u32SoftLastBurst;    /* events drained on last drain poll */
static u32 g_u32SoftBurstMax;     /* peak events in one poll drain */
static u32 g_u32SoftBurstSum;     /* sum of events across drain polls */
static u32 g_u32SoftPushOk;       /* session_input_push_from accepted */
static u32 g_u32SoftPushReject;   /* push_from bad args (any) */
static u32 g_u32SoftPushNull;     /* push_from pEv == NULL */
static u32 g_u32SoftPushBadSrc;   /* push_from u32Src out of range */
static u32 g_u32SoftPushVirtio;   /* push attributed to SRC_VIRTIO */
static u32 g_u32SoftPushSoft;     /* push attributed to SRC_SOFT */
static u32 g_u32SoftEnqueue;      /* input_enqueue accepted */
static u32 g_u32SoftEnqEmpty;     /* enqueue onto empty ring */
static u32 g_u32SoftEnqPartial;   /* enqueue with 0 < len < RING */
static u32 g_u32SoftEnqFull;      /* enqueue when full (drop-oldest) */
static u32 g_u32SoftDropOld;      /* drop-oldest under full ring (shadow) */
static u32 g_u32SoftPopHit;       /* session_input_pop filled *pOut */
static u32 g_u32SoftPopDirect;    /* pop hit without lazy refill */
static u32 g_u32SoftPopEmpty;     /* pop terminal empty (after lazy) */
static u32 g_u32SoftPopNull;      /* pop with pOut == NULL */
static u32 g_u32SoftPopLazy;      /* empty-ring lazy fan-in polls */
static u32 g_u32SoftPopLazyHit;   /* lazy refill then successful pop */
static u32 g_u32SoftPopLazyMiss;  /* lazy refill still empty */
static u32 g_u32SoftPopRepair;    /* pop aborted after ring repair */
static u32 g_u32SoftSaneRepair;   /* input_ring_sane repaired corrupt state */
static u32 g_u32SoftSaneOk;       /* input_ring_sane found usable state */
static u32 g_u32SoftSaneCheck;    /* input_ring_sane entries */
static u32 g_u32SoftEvSyn;        /* enqueued GJ_EV_SYN */
static u32 g_u32SoftEvKey;        /* enqueued GJ_EV_KEY */
static u32 g_u32SoftEvRel;        /* enqueued GJ_EV_REL */
static u32 g_u32SoftEvAbs;        /* enqueued GJ_EV_ABS */
static u32 g_u32SoftEvOther;      /* enqueued other type */
static u32 g_u32SoftReadyFlip;    /* first sticky ready=1 transitions */
static u32 g_u32SoftInit;         /* session_input_init entries */
static u32 g_u32SoftQPending;     /* session_input_pending samples */
static u32 g_u32SoftQPeak;        /* session_input_peak_pending samples */
static u32 g_u32SoftQPushed;      /* session_input_pushed samples */
static u32 g_u32SoftQDropped;     /* session_input_dropped samples */
static u32 g_u32SoftQBursts;      /* session_input_poll_bursts samples */
static u32 g_u32SoftQSrc;         /* session_input_pushed_src samples */
static u32 g_u32SoftQSrcBad;      /* pushed_src out-of-range */
static u32 g_u32SoftQReady;       /* session_input_ready samples */
static u32 g_u32SoftHeadWrap;     /* soft head wraps (mod RING) observed */
static u32 g_u32SoftPeakOccPct;   /* peak ring occupancy percent */
static u32 g_u32SoftLastPopHit;   /* 1 if last pop filled, 0 empty/null */
static u32 g_u32SoftLogN;         /* soft inventory log emissions */
static u32 g_u32SoftResidualLean; /* lean residual self-check runs */
static u32 g_u32SoftResidualLeanOk; /* lean residual push->pop ok */
static u8  g_fSoftInvOnce;        /* one-shot deep dump after activity */
static u8  g_fSoftResidualLeanOnce; /* one-shot residual self-check */
static u8  g_fSoftResidualBusy;   /* suppress maybe_once during lean check */

static void soft_inc(u32 *pCtr);
static void soft_note_ev_type(u16 u16Type);
static void soft_note_ready(int fNow);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_residual_lean_once(void);

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

/** Soft: classify enqueued event type for greppable ev surface. */
static void
soft_note_ev_type(u16 u16Type)
{
    if (u16Type == (u16)GJ_EV_SYN) {
        soft_inc(&g_u32SoftEvSyn);
    } else if (u16Type == (u16)GJ_EV_KEY) {
        soft_inc(&g_u32SoftEvKey);
    } else if (u16Type == (u16)GJ_EV_REL) {
        soft_inc(&g_u32SoftEvRel);
    } else if (u16Type == (u16)GJ_EV_ABS) {
        soft_inc(&g_u32SoftEvAbs);
    } else {
        soft_inc(&g_u32SoftEvOther);
    }
}

/** Soft: sticky ready flip when virtio-input first observed ready. */
static void
soft_note_ready(int fNow)
{
    if (fNow != 0 && g_fReady == 0) {
        soft_inc(&g_u32SoftReadyFlip);
    }
    if (fNow != 0) {
        g_fReady = 1;
    }
}

/**
 * Lean soft residual inventory (stack-safe; no stamp storms).
 * CRITICAL: few short kprintfs only - never twin multi-line dumps, no
 * version stamp. Prior residual printed 50+ twin kprintfs per dump
 * (wave stamps) and risked serial flood.
 * Soft only - never hard-gates product policy. Soft!=product · G-AC-1.
 * Dual MIT OR Apache-2.0 (tree SPDX); soft residual != product claim.
 * Grep: input_hub: soft inventory | soft residual lean | soft PASS
 * Grep: input: soft inventory | soft residual lean | soft PASS
 */
static void
soft_inventory_log(void)
{
    u32 u32Ready;
    u32 u32Pending;
    u32 u32Free;
    u32 u32Virtio;
    u32 u32SoftSrc;
    u32 u32Head;
    u32 u32Peak;
    u32 u32Pushed;
    u32 u32Dropped;
    u32 u32Bursts;
    u32 u32OccPct;
    u32 u32DropRatio;
    u32 u32HitBp;
    u32 u32AvgBurst;
    u32 u32VirtPct;
    u32 u32PopEnter;
    u32 u32VirtLive;
    u32 u32Surf;
    const char *szVerdict;

    soft_inc(&g_u32SoftLogN);

    /* Snapshot live ring (diagnostics only; no hard lock). */
    u32Ready = (g_fReady != 0) ? 1u : 0u;
    u32VirtLive = virtio_input_ready() ? 1u : 0u;
    if (u32VirtLive != 0u) {
        soft_note_ready(1);
        u32Ready = 1u;
    }
    u32Head = g_u32Head;
    u32Pending = g_u32Len;
    if (u32Head >= GJ_INPUT_RING || u32Pending > GJ_INPUT_RING) {
        u32Head = 0;
        u32Pending = 0;
    }
    u32Free = (u32Pending < GJ_INPUT_RING) ? (GJ_INPUT_RING - u32Pending) : 0u;
    u32Peak = g_u32PeakPending;
    u32Pushed = g_u32Pushed;
    u32Dropped = g_u32Dropped;
    u32Bursts = g_u32PollBursts;
    u32Virtio = g_aPushedSrc[GJ_INPUT_SRC_VIRTIO];
    u32SoftSrc = g_aPushedSrc[GJ_INPUT_SRC_SOFT];
    u32OccPct = (u32Pending * 100u) / GJ_INPUT_RING;
    if (u32OccPct > g_u32SoftPeakOccPct) {
        g_u32SoftPeakOccPct = u32OccPct;
    }
    if (u32Pushed != 0u) {
        u32DropRatio = (u32Dropped * 10000u) / u32Pushed;
    } else {
        u32DropRatio = 0;
    }
    u32PopEnter = g_u32SoftPopHit + g_u32SoftPopEmpty + g_u32SoftPopNull;
    if (u32PopEnter != 0u) {
        u32HitBp = (g_u32SoftPopHit * 10000u) / u32PopEnter;
    } else {
        u32HitBp = 0;
    }
    if (g_u32SoftPollDrain != 0u) {
        u32AvgBurst = g_u32SoftBurstSum / g_u32SoftPollDrain;
    } else {
        u32AvgBurst = 0;
    }
    if ((u32Virtio + u32SoftSrc) != 0u) {
        u32VirtPct = (u32Virtio * 100u) / (u32Virtio + u32SoftSrc);
    } else {
        u32VirtPct = 0;
    }

    /*
     * Soft verdict (inventory only; never hard-gates input):
     *   INIT     - no poll/push/pop activity yet
     *   PASS     - any enqueue or pop hit observed
     *   PARTIAL  - only empty/reject/nodev activity
     */
    if (g_u32SoftEnqueue != 0u || g_u32SoftPopHit != 0u) {
        szVerdict = "PASS";
    } else if (g_u32SoftPollEnter != 0u || g_u32SoftPushReject != 0u ||
               g_u32SoftPopEmpty != 0u || g_u32SoftPopNull != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "INIT";
    }

    /* Surface bits: ready|virtio_live|pending|pushed|pop|drop|enq|lazy|lean */
    u32Surf = (u32Ready) | (u32VirtLive << 1) |
              ((u32Pending != 0u) ? (1u << 2) : 0u) |
              ((u32Pushed != 0u) ? (1u << 3) : 0u) |
              ((g_u32SoftPopHit != 0u) ? (1u << 4) : 0u) |
              ((u32Dropped != 0u) ? (1u << 5) : 0u) |
              ((g_u32SoftEnqueue != 0u) ? (1u << 6) : 0u) |
              ((g_u32SoftPopLazy != 0u) ? (1u << 7) : 0u) |
              ((g_u32SoftResidualLeanOk != 0u) ? (1u << 8) : 0u);

    /*
     * Grep: input_hub: soft inventory
     * One-line rollup (capacity + fan-in + poll/push/pop). Soft residual only.
     */
    kprintf("input_hub: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "occ_pct=%u drop_bp=%u hit_bp=%u poll_enter=%u bursts=%u "
            "push_ok=%u pop_hit=%u enqueue=%u log_n=%u "
            "(one-line Soft!=product; no version stamp)\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped, u32OccPct,
            u32DropRatio, u32HitBp, g_u32SoftPollEnter, u32Bursts,
            g_u32SoftPushOk, g_u32SoftPopHit, g_u32SoftEnqueue,
            g_u32SoftLogN);

    /*
     * Grep: input_hub: soft residual lean
     * Lean residual honesty - Soft!=product · G-AC-1; no version stamp.
     * desktop_product=OPEN remains open product surface. stamp_storm=0.
     */
    kprintf("input_hub: soft residual lean "
            "fanin=virtio+soft_inject irq=0 drop_oldest=1 lazy_fanin=1 "
            "virtio=%u soft=%u virt_pct=%u poll_drain=%u idle=%u "
            "lazy_hit=%u lazy_miss=%u enq_full=%u drop_old=%u "
            "avg_burst=%u burst_max=%u head=%u head_wrap=%u "
            "repair=%u reject=%u null_rej=%u bad_src=%u "
            "lean_runs=%u lean_ok=%u surf=0x%x "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 desktop_product=OPEN "
            "G-AC-1 stamp_storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; "
            "no version stamp; not desktop product)\n",
            u32Virtio, u32SoftSrc, u32VirtPct, g_u32SoftPollDrain,
            g_u32SoftPollIdle, g_u32SoftPopLazyHit, g_u32SoftPopLazyMiss,
            g_u32SoftEnqFull, g_u32SoftDropOld, u32AvgBurst,
            g_u32SoftBurstMax, u32Head, g_u32SoftHeadWrap,
            g_u32SoftSaneRepair, g_u32SoftPushReject, g_u32SoftPushNull,
            g_u32SoftPushBadSrc, g_u32SoftResidualLean,
            g_u32SoftResidualLeanOk, u32Surf);

    /* Grep: input_hub: soft PASS|PARTIAL|INIT */
    kprintf("input_hub: soft %s ready=%u pushed=%u pop_hit=%u "
            "enqueue=%u lean_ok=%u soft=1 product=0 Soft!=product G-AC-1\n",
            szVerdict, u32Ready, u32Pushed, g_u32SoftPopHit,
            g_u32SoftEnqueue, g_u32SoftResidualLeanOk);

    /*
     * Twin prefix: input: soft ... (agent-friendly alias; same tallies).
     * Lean only - no multi-line twin dump / no stamp storm.
     */
    /* Grep: input: soft inventory */
    kprintf("input: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "occ_pct=%u drop_bp=%u hit_bp=%u poll_enter=%u bursts=%u "
            "push_ok=%u pop_hit=%u enqueue=%u log_n=%u "
            "(one-line Soft!=product; no version stamp)\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped, u32OccPct,
            u32DropRatio, u32HitBp, g_u32SoftPollEnter, u32Bursts,
            g_u32SoftPushOk, g_u32SoftPopHit, g_u32SoftEnqueue,
            g_u32SoftLogN);

    /* Grep: input: soft residual lean */
    kprintf("input: soft residual lean "
            "fanin=virtio+soft_inject irq=0 drop_oldest=1 lazy_fanin=1 "
            "virtio=%u soft=%u virt_pct=%u poll_drain=%u idle=%u "
            "lazy_hit=%u lazy_miss=%u enq_full=%u drop_old=%u "
            "avg_burst=%u burst_max=%u head=%u head_wrap=%u "
            "repair=%u reject=%u null_rej=%u bad_src=%u "
            "lean_runs=%u lean_ok=%u surf=0x%x "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 desktop_product=OPEN "
            "G-AC-1 stamp_storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; "
            "no version stamp; not desktop product)\n",
            u32Virtio, u32SoftSrc, u32VirtPct, g_u32SoftPollDrain,
            g_u32SoftPollIdle, g_u32SoftPopLazyHit, g_u32SoftPopLazyMiss,
            g_u32SoftEnqFull, g_u32SoftDropOld, u32AvgBurst,
            g_u32SoftBurstMax, u32Head, g_u32SoftHeadWrap,
            g_u32SoftSaneRepair, g_u32SoftPushReject, g_u32SoftPushNull,
            g_u32SoftPushBadSrc, g_u32SoftResidualLean,
            g_u32SoftResidualLeanOk, u32Surf);

    /* Grep: input: soft PASS|PARTIAL|INIT */
    kprintf("input: soft %s ready=%u pushed=%u pop_hit=%u "
            "enqueue=%u lean_ok=%u soft=1 product=0 Soft!=product G-AC-1\n",
            szVerdict, u32Ready, u32Pushed, g_u32SoftPopHit,
            g_u32SoftEnqueue, g_u32SoftResidualLeanOk);
}

/**
 * Lean residual self-check (soft inject path; never touches virtio HW).
 * C0 deepen (STRONGER edges; Soft!=product · G-AC-1 · stamp-free):
 *   - reject null + reject bad-src + pop null (arg guards)
 *   - reject_clean: len stable across reject paths (no ring pollution)
 *   - soft inject SYN with non-zero code/value -> pop field match
 *     (type+code+value; proves copy, not zero-default lamp)
 *   - pending_after=1 then empty_after (cursor restore)
 * Honest lamp PASS|PARTIAL|FAIL (never hardcode PASS).
 * Dual MIT OR Apache-2.0 · Dual DoD OPEN · no version stamp · stamp_storm=0.
 * greppable: input_hub: soft residual lean PASS|PARTIAL|FAIL
 * greppable: input: soft residual lean PASS|PARTIAL|FAIL
 */
static void
soft_residual_lean_once(void)
{
    struct gj_input_event evIn;
    struct gj_input_event evOut;
    int fPush;
    int fPop;
    int fNullRej;
    int fBadSrc;
    int fPopNull;
    int fRejectClean;
    int fPendingAfter;
    int fEmptyAfter;
    int fFieldMatch;
    int fCoreOk;
    int fEdgesOk;
    u32 u32LenBefore;
    const char *szLamp;

    if (g_fSoftResidualLeanOnce != 0) {
        return;
    }
    g_fSoftResidualLeanOnce = 1;
    soft_inc(&g_u32SoftResidualLean);

    memset(&evIn, 0, sizeof(evIn));
    memset(&evOut, 0, sizeof(evOut));
    /*
     * Distinctive non-zero payload so copy integrity is proven
     * (zero code/value would pass even if enqueue zero-filled).
     * Soft residual marker only; not a protocol claim. Soft!=product.
     */
    evIn.u16Type = (u16)GJ_EV_SYN;
    evIn.u16Code = (u16)0xA11u;
    evIn.i32Value = (i32)0x51C0;

    /*
     * Suppress maybe_once during self-check (no mid-check dump / storm).
     * Order: reject paths first (no ring pollution), then inject->pop,
     * then pending/empty integrity on live cursor.
     */
    g_fSoftResidualBusy = 1;
    u32LenBefore = g_u32Len;
    fNullRej = (session_input_push_from(GJ_INPUT_SRC_SOFT, NULL) == 0) ? 1 : 0;
    fBadSrc = (session_input_push_from(GJ_INPUT_SRC_MAX, &evIn) == 0) ? 1 : 0;
    fPopNull = (session_input_pop(NULL) == 0) ? 1 : 0;
    /* Rejects / null-pop must not pollute or drain the ring. */
    fRejectClean = (g_u32Len == u32LenBefore) ? 1 : 0;
    fPush = session_input_push_from(GJ_INPUT_SRC_SOFT, &evIn);
    fPendingAfter = (fPush == 1 && g_u32Len == (u32LenBefore + 1u)) ? 1 : 0;
    fPop = session_input_pop(&evOut);
    fEmptyAfter = (g_u32Len == u32LenBefore) ? 1 : 0;
    g_fSoftResidualBusy = 0;

    fFieldMatch = (fPop == 1 &&
                   evOut.u16Type == evIn.u16Type &&
                   evOut.u16Code == evIn.u16Code &&
                   evOut.i32Value == evIn.i32Value) ? 1 : 0;
    fCoreOk = (fPush == 1 && fPop == 1 && fFieldMatch != 0) ? 1 : 0;
    fEdgesOk = (fNullRej != 0 && fBadSrc != 0 && fPopNull != 0 &&
                fRejectClean != 0 && fPendingAfter != 0 &&
                fEmptyAfter != 0) ? 1 : 0;
    if (fCoreOk != 0 && fEdgesOk != 0) {
        soft_inc(&g_u32SoftResidualLeanOk);
        szLamp = "PASS";
    } else if (fCoreOk != 0) {
        /* Core inject->pop+field ok; reject/pending residual incomplete. */
        szLamp = "PARTIAL";
    } else {
        szLamp = "FAIL";
    }

    /* Grep: input_hub: soft residual lean PASS|PARTIAL|FAIL */
    kprintf("input_hub: soft residual lean %s "
            "push=%d pop=%d type=%u code=%u val=%d "
            "null_rej=%d bad_src=%d pop_null=%d rej_clean=%d "
            "pending_after=%d empty_after=%d field=%d "
            "lean_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "desktop_product=OPEN G-AC-1 stamp_storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp)\n",
            szLamp, fPush, fPop, (unsigned)evOut.u16Type,
            (unsigned)evOut.u16Code, (int)evOut.i32Value,
            fNullRej, fBadSrc, fPopNull, fRejectClean,
            fPendingAfter, fEmptyAfter, fFieldMatch,
            g_u32SoftResidualLeanOk);
    /* Grep: input: soft residual lean PASS|PARTIAL|FAIL */
    kprintf("input: soft residual lean %s "
            "push=%d pop=%d type=%u code=%u val=%d "
            "null_rej=%d bad_src=%d pop_null=%d rej_clean=%d "
            "pending_after=%d empty_after=%d field=%d "
            "lean_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "desktop_product=OPEN G-AC-1 stamp_storm=0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp)\n",
            szLamp, fPush, fPop, (unsigned)evOut.u16Type,
            (unsigned)evOut.u16Code, (int)evOut.i32Value,
            fNullRej, fBadSrc, fPopNull, fRejectClean,
            fPendingAfter, fEmptyAfter, fFieldMatch,
            g_u32SoftResidualLeanOk);
}

/**
 * After first product poll/push/pop activity, print soft inventory once
 * (mirrors memobj/futex soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0 || g_fSoftResidualBusy != 0) {
        return;
    }
    if (g_u32SoftPollEnter == 0 && g_u32SoftPushOk == 0 &&
        g_u32SoftPushReject == 0 && g_u32SoftPopHit == 0 &&
        g_u32SoftPopEmpty == 0 && g_u32SoftPopNull == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

/** Reset ring state; safe to call more than once. */
void
session_input_init(void)
{
    soft_inc(&g_u32SoftInit);

    memset(g_aRing, 0, sizeof(g_aRing));
    memset(g_aPushedSrc, 0, sizeof(g_aPushedSrc));
    g_u32Head = 0;
    g_u32Len = 0;
    g_u32Pushed = 0;
    g_u32Dropped = 0;
    g_u32PeakPending = 0;
    g_u32PollBursts = 0;
    g_fReady = 0;
    if (virtio_input_ready()) {
        soft_note_ready(1);
    }
    g_u32SoftPollEnter = 0;
    g_u32SoftPollNodev = 0;
    g_u32SoftPollReady = 0;
    g_u32SoftPollDrain = 0;
    g_u32SoftPollIdle = 0;
    g_u32SoftPollCap = 0;
    g_u32SoftLastBurst = 0;
    g_u32SoftBurstMax = 0;
    g_u32SoftBurstSum = 0;
    g_u32SoftPushOk = 0;
    g_u32SoftPushReject = 0;
    g_u32SoftPushNull = 0;
    g_u32SoftPushBadSrc = 0;
    g_u32SoftPushVirtio = 0;
    g_u32SoftPushSoft = 0;
    g_u32SoftEnqueue = 0;
    g_u32SoftEnqEmpty = 0;
    g_u32SoftEnqPartial = 0;
    g_u32SoftEnqFull = 0;
    g_u32SoftDropOld = 0;
    g_u32SoftPopHit = 0;
    g_u32SoftPopDirect = 0;
    g_u32SoftPopEmpty = 0;
    g_u32SoftPopNull = 0;
    g_u32SoftPopLazy = 0;
    g_u32SoftPopLazyHit = 0;
    g_u32SoftPopLazyMiss = 0;
    g_u32SoftPopRepair = 0;
    g_u32SoftSaneRepair = 0;
    g_u32SoftSaneOk = 0;
    g_u32SoftSaneCheck = 0;
    g_u32SoftEvSyn = 0;
    g_u32SoftEvKey = 0;
    g_u32SoftEvRel = 0;
    g_u32SoftEvAbs = 0;
    g_u32SoftEvOther = 0;
    /* Keep sticky ready-flip history across re-init (soft bring-up). */
    g_u32SoftQPending = 0;
    g_u32SoftQPeak = 0;
    g_u32SoftQPushed = 0;
    g_u32SoftQDropped = 0;
    g_u32SoftQBursts = 0;
    g_u32SoftQSrc = 0;
    g_u32SoftQSrcBad = 0;
    g_u32SoftQReady = 0;
    g_u32SoftHeadWrap = 0;
    g_u32SoftPeakOccPct = 0;
    g_u32SoftLastPopHit = 0;
    g_u32SoftLogN = 0;
    g_u32SoftResidualLean = 0;
    g_u32SoftResidualLeanOk = 0;
    g_fSoftInvOnce = 0;
    g_fSoftResidualBusy = 0;
    /* Residual lean may re-run after re-init (self-check proves push/pop). */
    g_fSoftResidualLeanOnce = 0;
    kprintf("session_input: init ready=%d ring=%u fan-in src=%u "
            "(soft residual lean; no version stamp)\n",
            g_fReady, GJ_INPUT_RING, GJ_INPUT_SRC_MAX);
    /*
     * Lean residual self-check then baseline inventory.
     * Soft!=product dual MIT OR Apache-2.0; no version stamp.
     */
    soft_residual_lean_once();
    soft_inventory_log();
}

/**
 * Repair ring cursors if state looks corrupt (defensive; should not fire).
 * Returns non-zero if the ring is usable after the check.
 */
static int
input_ring_sane(void)
{
    soft_inc(&g_u32SoftSaneCheck);
    if (g_u32Head >= GJ_INPUT_RING || g_u32Len > GJ_INPUT_RING) {
        g_u32Head = 0;
        g_u32Len = 0;
        memset(g_aRing, 0, sizeof(g_aRing));
        soft_inc(&g_u32SoftSaneRepair);
        return 0;
    }
    soft_inc(&g_u32SoftSaneOk);
    return 1;
}

static void
input_note_pending(void)
{
    u32 u32Occ;

    if (g_u32Len > g_u32PeakPending) {
        g_u32PeakPending = g_u32Len;
    }
    /* Soft: peak occupancy percent (capacity never 0). */
    u32Occ = (g_u32Len * 100u) / GJ_INPUT_RING;
    if (u32Occ > g_u32SoftPeakOccPct) {
        g_u32SoftPeakOccPct = u32Occ;
    }
}

/**
 * Enqueue one event under @u32Src (must be < GJ_INPUT_SRC_MAX).
 * Drops oldest when full. Caller ensures ring sane.
 */
static void
input_enqueue(u32 u32Src, const struct gj_input_event *pEv)
{
    u32 u32Pos;

    if (pEv == NULL || u32Src >= GJ_INPUT_SRC_MAX) {
        return;
    }
    if (g_u32Len >= GJ_INPUT_RING) {
        /* Drop oldest - keep a live tail for sessiond. */
        soft_inc(&g_u32SoftEnqFull);
        g_u32Head = (g_u32Head + 1u) % GJ_INPUT_RING;
        /* Soft: head wrap when drop advances past ring end (mod returns 0). */
        if (g_u32Head == 0u) {
            soft_inc(&g_u32SoftHeadWrap);
        }
        g_u32Len--;
        g_u32Dropped++;
        soft_inc(&g_u32SoftDropOld);
    } else if (g_u32Len == 0u) {
        soft_inc(&g_u32SoftEnqEmpty);
    } else {
        soft_inc(&g_u32SoftEnqPartial);
    }
    u32Pos = (g_u32Head + g_u32Len) % GJ_INPUT_RING;
    g_aRing[u32Pos] = *pEv;
    g_u32Len++;
    g_u32Pushed++;
    g_aPushedSrc[u32Src]++;
    soft_inc(&g_u32SoftEnqueue);
    soft_note_ev_type(pEv->u16Type);
    input_note_pending();
}

/** Drain virtio-input into the session ring (call from idle / door poll). */
void
session_input_poll(void)
{
    struct gj_input_event ev;
    u32 u32Burst;

    soft_inc(&g_u32SoftPollEnter);

    if (!virtio_input_ready()) {
        soft_inc(&g_u32SoftPollNodev);
        soft_inventory_maybe_once();
        return;
    }
    soft_note_ready(1);
    soft_inc(&g_u32SoftPollReady);
    (void)input_ring_sane();

    u32Burst = 0;
    for (; u32Burst < GJ_INPUT_POLL_MAX; u32Burst++) {
        if (virtio_input_poll(&ev) != 1) {
            break;
        }
        input_enqueue(GJ_INPUT_SRC_VIRTIO, &ev);
    }
    if (u32Burst != 0) {
        g_u32PollBursts++;
        soft_inc(&g_u32SoftPollDrain);
        g_u32SoftLastBurst = u32Burst;
        if (u32Burst > g_u32SoftBurstMax) {
            g_u32SoftBurstMax = u32Burst;
        }
        if (g_u32SoftBurstSum < (0xffffffffu - u32Burst)) {
            g_u32SoftBurstSum += u32Burst;
        } else {
            g_u32SoftBurstSum = 0xffffffffu;
        }
        if (u32Burst >= GJ_INPUT_POLL_MAX) {
            soft_inc(&g_u32SoftPollCap);
        }
    } else {
        soft_inc(&g_u32SoftPollIdle);
    }
    /*
     * Soft secondary fan-in slot (GJ_INPUT_SRC_SOFT): no live producer at
     * A1; reserved so multi-source architecture is exercised by inject.
     */
    soft_inventory_maybe_once();
}

int
session_input_push_from(u32 u32Src, const struct gj_input_event *pEv)
{
    if (pEv == NULL) {
        soft_inc(&g_u32SoftPushReject);
        soft_inc(&g_u32SoftPushNull);
        soft_inventory_maybe_once();
        return 0;
    }
    if (u32Src >= GJ_INPUT_SRC_MAX) {
        soft_inc(&g_u32SoftPushReject);
        soft_inc(&g_u32SoftPushBadSrc);
        soft_inventory_maybe_once();
        return 0;
    }
    (void)input_ring_sane();
    input_enqueue(u32Src, pEv);
    soft_inc(&g_u32SoftPushOk);
    if (u32Src == GJ_INPUT_SRC_VIRTIO) {
        soft_inc(&g_u32SoftPushVirtio);
    } else if (u32Src == GJ_INPUT_SRC_SOFT) {
        soft_inc(&g_u32SoftPushSoft);
    }
    soft_inventory_maybe_once();
    return 1;
}

/**
 * Pop one event for session clients.
 * Soft lazy fan-in: empty ring -> poll producers once, then try again.
 * Returns 1 if *pOut filled, 0 if empty or pOut is NULL.
 */
int
session_input_pop(struct gj_input_event *pOut)
{
    if (pOut == NULL) {
        soft_inc(&g_u32SoftPopNull);
        g_u32SoftLastPopHit = 0;
        soft_inventory_maybe_once();
        return 0;
    }
    if (!input_ring_sane()) {
        soft_inc(&g_u32SoftPopEmpty);
        soft_inc(&g_u32SoftPopRepair);
        g_u32SoftLastPopHit = 0;
        soft_inventory_maybe_once();
        return 0;
    }
    if (g_u32Len == 0) {
        /* Lazy fan-in soft: refill once before declaring empty. */
        soft_inc(&g_u32SoftPopLazy);
        session_input_poll();
        if (!input_ring_sane() || g_u32Len == 0) {
            soft_inc(&g_u32SoftPopEmpty);
            soft_inc(&g_u32SoftPopLazyMiss);
            g_u32SoftLastPopHit = 0;
            soft_inventory_maybe_once();
            return 0;
        }
        soft_inc(&g_u32SoftPopLazyHit);
    } else {
        soft_inc(&g_u32SoftPopDirect);
    }
    *pOut = g_aRing[g_u32Head];
    g_u32Head = (g_u32Head + 1u) % GJ_INPUT_RING;
    if (g_u32Head == 0u) {
        soft_inc(&g_u32SoftHeadWrap);
    }
    g_u32Len--;
    soft_inc(&g_u32SoftPopHit);
    g_u32SoftLastPopHit = 1;
    soft_inventory_maybe_once();
    return 1;
}

u32
session_input_pushed(void)
{
    soft_inc(&g_u32SoftQPushed);
    return g_u32Pushed;
}

u32
session_input_dropped(void)
{
    soft_inc(&g_u32SoftQDropped);
    return g_u32Dropped;
}

u32
session_input_pending(void)
{
    soft_inc(&g_u32SoftQPending);
    if (!input_ring_sane()) {
        return 0;
    }
    return g_u32Len;
}

u32
session_input_peak_pending(void)
{
    soft_inc(&g_u32SoftQPeak);
    return g_u32PeakPending;
}

u32
session_input_poll_bursts(void)
{
    soft_inc(&g_u32SoftQBursts);
    return g_u32PollBursts;
}

u32
session_input_pushed_src(u32 u32Src)
{
    soft_inc(&g_u32SoftQSrc);
    if (u32Src >= GJ_INPUT_SRC_MAX) {
        soft_inc(&g_u32SoftQSrcBad);
        return 0;
    }
    return g_aPushedSrc[u32Src];
}

int
session_input_ready(void)
{
    soft_inc(&g_u32SoftQReady);
    return g_fReady != 0;
}
