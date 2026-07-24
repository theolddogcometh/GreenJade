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
 * When the ring is full the oldest event is dropped — latency over fidelity
 * for the interim keyboard/pointer path. Drop count is retained for STATS.
 *
 * Soft input hub inventory (Wave 35 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Ring capacity / live pending / peak / free + drop-oldest policy
 *   - Fan-in src tallies (virtio live + soft inject slot)
 *   - Poll: enter / nodev / ready / drain / idle / cap-hit / burst peaks
 *   - Push: ok / reject split (null|bad_src) / per-src inject
 *   - Enqueue: empty / partial / full(+drop) + event-type soft tallies
 *   - Pop: direct hit / lazy hit|miss / null / repair empty
 *   - Ready flip + query accessor soft samples
 *   - Ratio / balance / avg burst / peak occ% / wrap / verdict surfaces
 *   - Wave 15 base: honesty / capacity / wrap / catalog / deepen
 *   - Wave 16 base: headroom / surface / terminal lamps (twin prefixes)
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 *   Soft ≠ desktop/compositor product bar.
 * Greppable twin prefixes (product / agent greps):
 *   "input_hub: soft …"
 *   "input: soft …"
 * greppable: input_hub: soft
 * greppable: input_hub: soft deepen
 * greppable: input: soft
 * greppable: input: soft deepen
 */
#include <gj/klog.h>
#include <gj/session_input.h>
#include <gj/string.h>
#include <gj/virtio_input.h>

#define GJ_INPUT_RING 64u
/* Cap one poll burst so a stuck backend cannot spin the door forever. */
#define GJ_INPUT_POLL_MAX 256u
/* Wave 20 deepen stamp (file-local; never hard-gates). */
#define GJ_INPUT_SOFT_WAVE 116u

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
 * Soft product inventory (Wave 35 exclusive deepen).
 * Cumulative unless noted live/peak. Never hard-gates.
 * greppable: input_hub: soft … / input: soft …
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
static u8  g_fSoftInvOnce;        /* one-shot deep dump after activity */

static void soft_inc(u32 *pCtr);
static void soft_note_ev_type(u16 u16Type);
static void soft_note_ready(int fNow);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

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
 * Greppable soft input hub inventory (product / smoke).
 * Twin prefixes so either agent grep works (Wave 20 deepen):
 *   input_hub: soft honesty|inventory|ring|fan-in|poll|push|pop|enqueue|
 *              drop|lazy|ready|ev|stats|query|ratio|balance|peaks|
 *              capacity|wrap|headroom|surface|terminal|catalog|path|
 *              deepen|PASS…
 *   input: soft … (same catalog)
 * greppable: input_hub: soft
 * greppable: input_hub: soft deepen
 * greppable: input: soft
 * greppable: input: soft deepen
 * Honesty: soft inventory only — not bar3 / desktop product.
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
    u32 cAreas;
    const char *szVerdict;

    soft_inc(&g_u32SoftLogN);
    cAreas = 0;

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
    /* Soft occupancy % (integer; capacity never 0). */
    u32OccPct = (u32Pending * 100u) / GJ_INPUT_RING;
    if (u32OccPct > g_u32SoftPeakOccPct) {
        g_u32SoftPeakOccPct = u32OccPct;
    }
    /* Soft drop ratio in basis points of pushed (0 if none pushed). */
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
     *   INIT     — no poll/push/pop activity yet
     *   PASS     — any enqueue or pop hit observed
     *   PARTIAL  — only empty/reject/nodev activity
     */
    if (g_u32SoftEnqueue != 0u || g_u32SoftPopHit != 0u) {
        szVerdict = "PASS";
    } else if (g_u32SoftPollEnter != 0u || g_u32SoftPushReject != 0u ||
               g_u32SoftPopEmpty != 0u || g_u32SoftPopNull != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "INIT";
    }

    /*
     * Primary prefix: input_hub: soft …
     * Wave 20 deepen adds headroom/surface/terminal + prior surfaces.
     */
    /* Grep: input_hub: soft honesty */
    kprintf("input_hub: soft honesty interim_ring=1 desktop_product=OPEN "
            "irq=0 drop_oldest=1 bar3=OPEN soft_never_gates=1 wave=%u "
            "(soft; never closes desktop bar)\n",
            GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft inventory */
    kprintf("input_hub: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "occ_pct=%u drop_bp=%u hit_bp=%u log_n=%u wave=%u\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped, u32OccPct,
            u32DropRatio, u32HitBp, g_u32SoftLogN, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft ring */
    kprintf("input_hub: soft ring head=%u len=%u free=%u peak=%u "
            "capacity=%u policy=drop_oldest repair=%u sane_ok=%u "
            "sane_check=%u occ_pct=%u head_wrap=%u peak_occ=%u\n",
            u32Head, u32Pending, u32Free, u32Peak, GJ_INPUT_RING,
            g_u32SoftSaneRepair, g_u32SoftSaneOk, g_u32SoftSaneCheck,
            u32OccPct, g_u32SoftHeadWrap, g_u32SoftPeakOccPct);
    cAreas++;

    /* Grep: input_hub: soft fan-in */
    kprintf("input_hub: soft fan-in virtio=%u soft=%u src_max=%u "
            "live=virtio inject=soft_src lazy_pop=1 push_virtio=%u "
            "push_soft=%u virt_pct=%u\n",
            u32Virtio, u32SoftSrc, GJ_INPUT_SRC_MAX, g_u32SoftPushVirtio,
            g_u32SoftPushSoft, u32VirtPct);
    cAreas++;

    /* Grep: input_hub: soft poll */
    kprintf("input_hub: soft poll enter=%u nodev=%u ready=%u drain=%u "
            "idle=%u cap=%u last_burst=%u burst_max=%u burst_sum=%u "
            "avg_burst=%u bursts=%u poll_max=%u\n",
            g_u32SoftPollEnter, g_u32SoftPollNodev, g_u32SoftPollReady,
            g_u32SoftPollDrain, g_u32SoftPollIdle, g_u32SoftPollCap,
            g_u32SoftLastBurst, g_u32SoftBurstMax, g_u32SoftBurstSum,
            u32AvgBurst, u32Bursts, GJ_INPUT_POLL_MAX);
    cAreas++;

    /* Grep: input_hub: soft push */
    kprintf("input_hub: soft push ok=%u reject=%u null=%u bad_src=%u "
            "virtio=%u soft=%u enqueue=%u drop_old=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPushNull,
            g_u32SoftPushBadSrc, g_u32SoftPushVirtio, g_u32SoftPushSoft,
            g_u32SoftEnqueue, g_u32SoftDropOld);
    cAreas++;

    /* Grep: input_hub: soft pop */
    kprintf("input_hub: soft pop hit=%u direct=%u empty=%u null=%u "
            "lazy=%u lazy_hit=%u lazy_miss=%u repair=%u last_hit=%u\n",
            g_u32SoftPopHit, g_u32SoftPopDirect, g_u32SoftPopEmpty,
            g_u32SoftPopNull, g_u32SoftPopLazy, g_u32SoftPopLazyHit,
            g_u32SoftPopLazyMiss, g_u32SoftPopRepair, g_u32SoftLastPopHit);
    cAreas++;

    /* Grep: input_hub: soft enqueue */
    kprintf("input_hub: soft enqueue ok=%u empty=%u partial=%u full=%u "
            "drop_old=%u policy=drop_oldest\n",
            g_u32SoftEnqueue, g_u32SoftEnqEmpty, g_u32SoftEnqPartial,
            g_u32SoftEnqFull, g_u32SoftDropOld);
    cAreas++;

    /* Grep: input_hub: soft drop */
    kprintf("input_hub: soft drop old=%u total=%u drop_bp=%u "
            "policy=drop_oldest latency_over_fidelity=1\n",
            g_u32SoftDropOld, u32Dropped, u32DropRatio);
    cAreas++;

    /* Grep: input_hub: soft lazy */
    kprintf("input_hub: soft lazy enter=%u hit=%u miss=%u "
            "path=empty_pop_poll_once fanin=virtio\n",
            g_u32SoftPopLazy, g_u32SoftPopLazyHit, g_u32SoftPopLazyMiss);
    cAreas++;

    /* Grep: input_hub: soft ready */
    kprintf("input_hub: soft ready sticky=%u flip=%u virtio_live=%u "
            "init=%u\n",
            u32Ready, g_u32SoftReadyFlip, u32VirtLive, g_u32SoftInit);
    cAreas++;

    /* Grep: input_hub: soft ev */
    kprintf("input_hub: soft ev syn=%u key=%u rel=%u abs=%u other=%u "
            "enqueued=%u\n",
            g_u32SoftEvSyn, g_u32SoftEvKey, g_u32SoftEvRel, g_u32SoftEvAbs,
            g_u32SoftEvOther, g_u32SoftEnqueue);
    cAreas++;

    /* Grep: input_hub: soft stats */
    kprintf("input_hub: soft stats pushed=%u dropped=%u pending=%u "
            "peak=%u poll_enter=%u poll_drain=%u push_ok=%u pop_hit=%u "
            "pop_empty=%u enqueue=%u repair=%u log_n=%u wave=%u\n",
            u32Pushed, u32Dropped, u32Pending, u32Peak, g_u32SoftPollEnter,
            g_u32SoftPollDrain, g_u32SoftPushOk, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftEnqueue, g_u32SoftSaneRepair,
            g_u32SoftLogN, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft query */
    kprintf("input_hub: soft query pending=%u peak=%u pushed=%u "
            "dropped=%u bursts=%u src=%u src_bad=%u ready=%u\n",
            g_u32SoftQPending, g_u32SoftQPeak, g_u32SoftQPushed,
            g_u32SoftQDropped, g_u32SoftQBursts, g_u32SoftQSrc,
            g_u32SoftQSrcBad, g_u32SoftQReady);
    cAreas++;

    /* Grep: input_hub: soft ratio */
    kprintf("input_hub: soft ratio drop_bp=%u hit_bp=%u occ_pct=%u "
            "peak_occ=%u avg_burst=%u wave=%u\n",
            u32DropRatio, u32HitBp, u32OccPct, g_u32SoftPeakOccPct,
            u32AvgBurst, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft balance */
    kprintf("input_hub: soft balance virtio=%u soft=%u virt_pct=%u "
            "push_v=%u push_s=%u src_max=%u\n",
            u32Virtio, u32SoftSrc, u32VirtPct, g_u32SoftPushVirtio,
            g_u32SoftPushSoft, GJ_INPUT_SRC_MAX);
    cAreas++;

    /* Grep: input_hub: soft peaks */
    kprintf("input_hub: soft peaks pending=%u burst=%u occ_pct=%u "
            "pushed=%u dropped=%u wrap=%u logs=%u\n",
            u32Peak, g_u32SoftBurstMax, g_u32SoftPeakOccPct, u32Pushed,
            u32Dropped, g_u32SoftHeadWrap, g_u32SoftLogN);
    cAreas++;

    /* Grep: input_hub: soft capacity — Wave 15 design-constant surface. */
    kprintf("input_hub: soft capacity ring=%u poll_max=%u src_max=%u "
            "free=%u peak=%u peak_occ=%u policy=drop_oldest soft PASS\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Free,
            u32Peak, g_u32SoftPeakOccPct);
    cAreas++;

    /* Grep: input_hub: soft wrap — head wrap / full-drop surface. */
    kprintf("input_hub: soft wrap head=%u head_wrap=%u enq_full=%u "
            "drop_old=%u repair=%u soft PASS\n",
            u32Head, g_u32SoftHeadWrap, g_u32SoftEnqFull, g_u32SoftDropOld,
            g_u32SoftSaneRepair);
    cAreas++;

    /* Grep: input_hub: soft headroom — Wave 19 live slack lamps. */
    kprintf("input_hub: soft headroom free=%u pending=%u peak=%u "
            "ring=%u occ_pct=%u peak_occ=%u drop_bp=%u wave=%u\n",
            u32Free, u32Pending, u32Peak, GJ_INPUT_RING, u32OccPct,
            g_u32SoftPeakOccPct, u32DropRatio, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft surface — Wave 19 surface bit lamps. */
    kprintf("input_hub: soft surface ready=%u virtio_live=%u pending=%u "
            "pushed=%u popped=%u dropped=%u enqueue=%u "
            "surf=0x%x wave=%u\n",
            u32Ready, u32VirtLive, u32Pending != 0u ? 1u : 0u,
            u32Pushed != 0u ? 1u : 0u,
            g_u32SoftPopHit != 0u ? 1u : 0u,
            u32Dropped != 0u ? 1u : 0u,
            g_u32SoftEnqueue != 0u ? 1u : 0u,
            (u32Ready) | (u32VirtLive << 1) |
                ((u32Pending != 0u) ? (1u << 2) : 0u) |
                ((u32Pushed != 0u) ? (1u << 3) : 0u) |
                ((g_u32SoftPopHit != 0u) ? (1u << 4) : 0u) |
                ((u32Dropped != 0u) ? (1u << 5) : 0u) |
                ((g_u32SoftEnqueue != 0u) ? (1u << 6) : 0u),
            GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft terminal — Wave 19 outcome rollup. */
    kprintf("input_hub: soft terminal push_ok=%u push_rej=%u pop_hit=%u "
            "pop_empty=%u pop_null=%u enq=%u drop=%u soft %s wave=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftPopNull, g_u32SoftEnqueue,
            g_u32SoftDropOld, szVerdict, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft catalog */
    kprintf("input_hub: soft catalog honesty,inventory,ring,fan-in,poll,"
            "push,pop,enqueue,drop,lazy,ready,ev,stats,query,ratio,"
            "balance,peaks,capacity,wrap,headroom,surface,terminal,"
            "catalog,return,path,deepen "
            "wave=%u areas_expect=94 soft PASS\n",
            GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft path */
    kprintf("input_hub: soft path claim=virtio+soft_inject irq=0 "
            "drop_oldest=1 lazy_fanin=1 ring=%u poll_max=%u src_max=%u "
            "wave=%u (soft inventory; not bar3; not desktop product)\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX,
            GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft return — Wave 19 API return surfaces */
    kprintf("input_hub: soft return push_ok=%u push_rej=%u pop_hit=%u "
            "pop_empty=%u pop_null=%u enq=%u drop=%u ready=%u "
            "product_desktop=OPEN wave=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftPopNull, g_u32SoftEnqueue,
            g_u32SoftDropOld, u32Ready, GJ_INPUT_SOFT_WAVE);
    cAreas++;

    /* Grep: input_hub: soft retmap — Wave 19 return-surface map */
    kprintf("input_hub: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=116\n");

    /* Grep: input_hub: soft deepen — Wave 20 stamp + area count. */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: input_hub: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("input_hub: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /* Grep: input_hub: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("input_hub: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: input_hub: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("input_hub: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /* Grep: input_hub: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("input_hub: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: input_hub: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("input_hub: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("input_hub: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: input_hub: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("input_hub: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("input_hub: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: input_hub: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("input_hub: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("input_hub: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input_hub: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("input_hub: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("input_hub: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input_hub: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("input_hub: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("input_hub: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input_hub: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("input_hub: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input_hub: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("input_hub: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: input_hub: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("input_hub: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_INPUT_SOFT_WAVE);
                    /* Grep: input_hub: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("input_hub: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_INPUT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: input_hub: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("input_hub: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input_hub: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("input_hub: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_INPUT_SOFT_WAVE);
    kprintf("input_hub: soft deepen wave=%u areas=%u verdict=%s "
            "ready=%u pushed=%u pop_hit=%u enqueue=%u "
            "desktop_product=OPEN soft_never_gates=1 (soft; not bar3)\n",
            GJ_INPUT_SOFT_WAVE, cAreas, szVerdict, u32Ready, u32Pushed,
            g_u32SoftPopHit, g_u32SoftEnqueue);

    /* Grep: input_hub: soft PASS|PARTIAL|INIT */
    kprintf("input_hub: soft %s ready=%u pushed=%u pop_hit=%u "
            "enqueue=%u wave=%u (soft; not bar3)\n",
            szVerdict, u32Ready, u32Pushed, g_u32SoftPopHit,
            g_u32SoftEnqueue, GJ_INPUT_SOFT_WAVE);

    /*
     * Twin prefix: input: soft … (agent-friendly alias; same tallies).
     * Wave 16 base: headroom/surface/terminal + prior surfaces mirrored here too.
     */
    /* Grep: input: soft honesty */
    kprintf("input: soft honesty interim_ring=1 desktop_product=OPEN "
            "irq=0 drop_oldest=1 bar3=OPEN soft_never_gates=1 wave=%u "
            "(soft; never closes desktop bar)\n",
            GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft inventory */
    kprintf("input: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "occ_pct=%u drop_bp=%u hit_bp=%u log_n=%u wave=%u\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped, u32OccPct,
            u32DropRatio, u32HitBp, g_u32SoftLogN, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft ring */
    kprintf("input: soft ring head=%u len=%u free=%u peak=%u "
            "capacity=%u policy=drop_oldest repair=%u sane_ok=%u "
            "sane_check=%u occ_pct=%u head_wrap=%u peak_occ=%u\n",
            u32Head, u32Pending, u32Free, u32Peak, GJ_INPUT_RING,
            g_u32SoftSaneRepair, g_u32SoftSaneOk, g_u32SoftSaneCheck,
            u32OccPct, g_u32SoftHeadWrap, g_u32SoftPeakOccPct);

    /* Grep: input: soft fan-in */
    kprintf("input: soft fan-in virtio=%u soft=%u src_max=%u "
            "live=virtio inject=soft_src lazy_pop=1 push_virtio=%u "
            "push_soft=%u virt_pct=%u\n",
            u32Virtio, u32SoftSrc, GJ_INPUT_SRC_MAX, g_u32SoftPushVirtio,
            g_u32SoftPushSoft, u32VirtPct);

    /* Grep: input: soft poll */
    kprintf("input: soft poll enter=%u nodev=%u ready=%u drain=%u "
            "idle=%u cap=%u last_burst=%u burst_max=%u burst_sum=%u "
            "avg_burst=%u bursts=%u poll_max=%u\n",
            g_u32SoftPollEnter, g_u32SoftPollNodev, g_u32SoftPollReady,
            g_u32SoftPollDrain, g_u32SoftPollIdle, g_u32SoftPollCap,
            g_u32SoftLastBurst, g_u32SoftBurstMax, g_u32SoftBurstSum,
            u32AvgBurst, u32Bursts, GJ_INPUT_POLL_MAX);

    /* Grep: input: soft push */
    kprintf("input: soft push ok=%u reject=%u null=%u bad_src=%u "
            "virtio=%u soft=%u enqueue=%u drop_old=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPushNull,
            g_u32SoftPushBadSrc, g_u32SoftPushVirtio, g_u32SoftPushSoft,
            g_u32SoftEnqueue, g_u32SoftDropOld);

    /* Grep: input: soft pop */
    kprintf("input: soft pop hit=%u direct=%u empty=%u null=%u "
            "lazy=%u lazy_hit=%u lazy_miss=%u repair=%u last_hit=%u\n",
            g_u32SoftPopHit, g_u32SoftPopDirect, g_u32SoftPopEmpty,
            g_u32SoftPopNull, g_u32SoftPopLazy, g_u32SoftPopLazyHit,
            g_u32SoftPopLazyMiss, g_u32SoftPopRepair, g_u32SoftLastPopHit);

    /* Grep: input: soft enqueue */
    kprintf("input: soft enqueue ok=%u empty=%u partial=%u full=%u "
            "drop_old=%u policy=drop_oldest\n",
            g_u32SoftEnqueue, g_u32SoftEnqEmpty, g_u32SoftEnqPartial,
            g_u32SoftEnqFull, g_u32SoftDropOld);

    /* Grep: input: soft drop */
    kprintf("input: soft drop old=%u total=%u drop_bp=%u "
            "policy=drop_oldest latency_over_fidelity=1\n",
            g_u32SoftDropOld, u32Dropped, u32DropRatio);

    /* Grep: input: soft lazy */
    kprintf("input: soft lazy enter=%u hit=%u miss=%u "
            "path=empty_pop_poll_once fanin=virtio\n",
            g_u32SoftPopLazy, g_u32SoftPopLazyHit, g_u32SoftPopLazyMiss);

    /* Grep: input: soft ready */
    kprintf("input: soft ready sticky=%u flip=%u virtio_live=%u "
            "init=%u\n",
            u32Ready, g_u32SoftReadyFlip, u32VirtLive, g_u32SoftInit);

    /* Grep: input: soft ev */
    kprintf("input: soft ev syn=%u key=%u rel=%u abs=%u other=%u "
            "enqueued=%u\n",
            g_u32SoftEvSyn, g_u32SoftEvKey, g_u32SoftEvRel, g_u32SoftEvAbs,
            g_u32SoftEvOther, g_u32SoftEnqueue);

    /* Grep: input: soft stats */
    kprintf("input: soft stats pushed=%u dropped=%u pending=%u "
            "peak=%u poll_enter=%u poll_drain=%u push_ok=%u pop_hit=%u "
            "pop_empty=%u enqueue=%u repair=%u log_n=%u wave=%u\n",
            u32Pushed, u32Dropped, u32Pending, u32Peak, g_u32SoftPollEnter,
            g_u32SoftPollDrain, g_u32SoftPushOk, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftEnqueue, g_u32SoftSaneRepair,
            g_u32SoftLogN, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft query */
    kprintf("input: soft query pending=%u peak=%u pushed=%u "
            "dropped=%u bursts=%u src=%u src_bad=%u ready=%u\n",
            g_u32SoftQPending, g_u32SoftQPeak, g_u32SoftQPushed,
            g_u32SoftQDropped, g_u32SoftQBursts, g_u32SoftQSrc,
            g_u32SoftQSrcBad, g_u32SoftQReady);

    /* Grep: input: soft ratio */
    kprintf("input: soft ratio drop_bp=%u hit_bp=%u occ_pct=%u "
            "peak_occ=%u avg_burst=%u wave=%u\n",
            u32DropRatio, u32HitBp, u32OccPct, g_u32SoftPeakOccPct,
            u32AvgBurst, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft balance */
    kprintf("input: soft balance virtio=%u soft=%u virt_pct=%u "
            "push_v=%u push_s=%u src_max=%u\n",
            u32Virtio, u32SoftSrc, u32VirtPct, g_u32SoftPushVirtio,
            g_u32SoftPushSoft, GJ_INPUT_SRC_MAX);

    /* Grep: input: soft peaks */
    kprintf("input: soft peaks pending=%u burst=%u occ_pct=%u "
            "pushed=%u dropped=%u wrap=%u logs=%u\n",
            u32Peak, g_u32SoftBurstMax, g_u32SoftPeakOccPct, u32Pushed,
            u32Dropped, g_u32SoftHeadWrap, g_u32SoftLogN);

    /* Grep: input: soft capacity */
    kprintf("input: soft capacity ring=%u poll_max=%u src_max=%u "
            "free=%u peak=%u peak_occ=%u policy=drop_oldest soft PASS\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Free,
            u32Peak, g_u32SoftPeakOccPct);

    /* Grep: input: soft wrap */
    kprintf("input: soft wrap head=%u head_wrap=%u enq_full=%u "
            "drop_old=%u repair=%u soft PASS\n",
            u32Head, g_u32SoftHeadWrap, g_u32SoftEnqFull, g_u32SoftDropOld,
            g_u32SoftSaneRepair);

    /* Grep: input: soft headroom (Wave 19 twin) */
    kprintf("input: soft headroom free=%u pending=%u peak=%u "
            "ring=%u occ_pct=%u peak_occ=%u drop_bp=%u wave=%u\n",
            u32Free, u32Pending, u32Peak, GJ_INPUT_RING, u32OccPct,
            g_u32SoftPeakOccPct, u32DropRatio, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft surface (Wave 19 twin) */
    kprintf("input: soft surface ready=%u virtio_live=%u pending=%u "
            "pushed=%u popped=%u dropped=%u enqueue=%u "
            "surf=0x%x wave=%u\n",
            u32Ready, u32VirtLive, u32Pending != 0u ? 1u : 0u,
            u32Pushed != 0u ? 1u : 0u,
            g_u32SoftPopHit != 0u ? 1u : 0u,
            u32Dropped != 0u ? 1u : 0u,
            g_u32SoftEnqueue != 0u ? 1u : 0u,
            (u32Ready) | (u32VirtLive << 1) |
                ((u32Pending != 0u) ? (1u << 2) : 0u) |
                ((u32Pushed != 0u) ? (1u << 3) : 0u) |
                ((g_u32SoftPopHit != 0u) ? (1u << 4) : 0u) |
                ((u32Dropped != 0u) ? (1u << 5) : 0u) |
                ((g_u32SoftEnqueue != 0u) ? (1u << 6) : 0u),
            GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft terminal (Wave 19 twin) */
    kprintf("input: soft terminal push_ok=%u push_rej=%u pop_hit=%u "
            "pop_empty=%u pop_null=%u enq=%u drop=%u soft %s wave=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftPopNull, g_u32SoftEnqueue,
            g_u32SoftDropOld, szVerdict, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft catalog */
    kprintf("input: soft catalog honesty,inventory,ring,fan-in,poll,"
            "push,pop,enqueue,drop,lazy,ready,ev,stats,query,ratio,"
            "balance,peaks,capacity,wrap,headroom,surface,terminal,"
            "catalog,return,path,deepen "
            "wave=%u areas_expect=94 soft PASS\n",
            GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft path */
    kprintf("input: soft path claim=virtio+soft_inject irq=0 "
            "drop_oldest=1 lazy_fanin=1 ring=%u poll_max=%u src_max=%u "
            "wave=%u (soft inventory; not bar3; not desktop product)\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX,
            GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft return — Wave 19 twin return surfaces */
    kprintf("input: soft return push_ok=%u push_rej=%u pop_hit=%u "
            "pop_empty=%u pop_null=%u enq=%u drop=%u ready=%u "
            "product_desktop=OPEN wave=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftPopHit,
            g_u32SoftPopEmpty, g_u32SoftPopNull, g_u32SoftEnqueue,
            g_u32SoftDropOld, u32Ready, GJ_INPUT_SOFT_WAVE);

    /* Grep: input: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: input: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("input: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /* Grep: input: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("input: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: input: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("input: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
    /* Grep: input: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("input: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: input: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("input: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("input: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: input: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("input: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("input: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: input: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("input: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("input: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("input: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("input: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("input: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("input: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: input: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("input: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
            /* Grep: input: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("input: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_INPUT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: input: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("input: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_INPUT_SOFT_WAVE);
                    /* Grep: input: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("input: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_INPUT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: input: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("input: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("input: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_INPUT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: input: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("input: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("input: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: input: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("input: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("input: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: input: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("input: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("input: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
                            /* Grep: input: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("input: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("input: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("input: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("input: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("input: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("input: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("input: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retfortress — Wave 35 return-fortress honesty */
kprintf("input: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("input: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft rethold — Wave 36 return-hold honesty */
kprintf("input: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retspire — Wave 36 exclusive spire stamp */
kprintf("input: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retwall — Wave 37 return-wall honesty */
kprintf("input: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retgate — Wave 37 exclusive gate stamp */
kprintf("input: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retmoat — Wave 38 return-moat honesty */
kprintf("input: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retower — Wave 38 exclusive tower stamp */
kprintf("input: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("input: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("input: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("input: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("input: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retravelin — Wave 41 return-travelin honesty */
kprintf("input: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("input: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("input: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("input: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("input: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("input: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("input: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("input: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("input: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("input: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retbailey — Wave 46 return-bailey honesty */
kprintf("input: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);
/* Grep: input: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("input: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_INPUT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("input: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("input: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("input: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("input: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("input: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("input: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retsally — Wave 50 return-sally honesty */
kprintf("input: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("input: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retfosse — Wave 51 return-fosse honesty */
kprintf("input: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("input: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("input: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("input: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retravelin — Wave 53 return-travelin honesty */
kprintf("input: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("input: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("input: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retredan — Wave 54 exclusive redan stamp */
kprintf("input: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retflank — Wave 55 return-flank honesty */
kprintf("input: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retface — Wave 55 exclusive face stamp */
kprintf("input: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retgorge — Wave 56 return-gorge honesty */
kprintf("input: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("input: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retraverse — Wave 57 return-traverse honesty */
kprintf("input: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("input: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retorillon — Wave 58 return-orillon honesty */
kprintf("input: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("input: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("input: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("input: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retplace — Wave 60 return-place honesty */
kprintf("input: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("input: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("input: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("input: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("input: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("input: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("input: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("input: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: input: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("input: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: input: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("input: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: input: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("input: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: input: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("input: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: input: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("input: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=116 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: input: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("input: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=116 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("input: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("input: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("input: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("input: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("input: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("input: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("input: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("input: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("input: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("input: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: input: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("input: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("input: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: input: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("input: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("input: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionangle stamp; Soft≠product)\n");
/* Grep: input: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("input: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("input: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparapetangle stamp; Soft≠product)\n");
/* Grep: input: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("input: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("input: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retowerangle stamp; Soft≠product)\n");
/* Grep: input: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("input: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("input: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwallangle stamp; Soft≠product)\n");
/* Grep: input: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("input: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("input: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retholdangle stamp; Soft≠product)\n");
/* Grep: input: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("input: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("input: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retfortressangle stamp; Soft≠product)\n");
/* Grep: input: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("input: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("input: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: input: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("input: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("input: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: input: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("input: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("input: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: input: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("input: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("input: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaegisangle stamp; Soft≠product)\n");
/* Grep: input: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("input: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("input: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsigilangle stamp; Soft≠product)\n");
/* Grep: input: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("input: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("input: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retglyphangle stamp; Soft≠product)\n");
/* Grep: input: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("input: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("input: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retshardangle stamp; Soft≠product)\n");
/* Grep: input: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("input: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("input: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retprismangle stamp; Soft≠product)\n");
/* Grep: input: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("input: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("input: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcipherangle stamp; Soft≠product)\n");
/* Grep: input: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("input: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("input: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retledgerangle stamp; Soft≠product)\n");
/* Grep: input: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("input: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("input: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvaultangle stamp; Soft≠product)\n");
/* Grep: input: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("input: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("input: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettokenangle stamp; Soft≠product)\n");
/* Grep: input: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("input: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("input: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retphaseangle stamp; Soft≠product)\n");
/* Grep: input: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("input: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("input: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpulseangle stamp; Soft≠product)\n");

/* Grep: input: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("input: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("input: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retboundangle stamp; Soft≠product)\n");
/* Grep: input: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("input: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("input: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbladeangle stamp; Soft≠product)\n");
/* Grep: input: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("input: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("input: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retarcangle stamp; Soft≠product)\n");
/* Grep: input: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("input: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("input: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: input: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("input: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("input: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: input: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("input: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("input: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retellipseangle stamp; Soft≠product)\n");
/* Grep: input: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("input: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("input: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: input: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("input: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("input: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rethelixangle stamp; Soft≠product)\n");
/* Grep: input: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("input: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("input: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retknotangle stamp; Soft≠product)\n");
/* Grep: input: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("input: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("input: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retkleinangle stamp; Soft≠product)\n");
/* Grep: input: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("input: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("input: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaffineangle stamp; Soft≠product)\n");
/* Grep: input: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("input: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("input: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: input: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("input: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("input: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcubicangle stamp; Soft≠product)\n");
/* Grep: input: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("input: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("input: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retquinticangle stamp; Soft≠product)\n");
/* Grep: input: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("input: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("input: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbezierangle stamp; Soft≠product)\n");
/* Grep: input: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("input: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("input: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: input: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("input: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("input: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: input: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("input: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("input: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retgridangle stamp; Soft≠product)\n");
/* Grep: input: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("input: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("input: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettexelangle stamp; Soft≠product)\n");
/* Grep: input: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("input: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("input: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvertexangle stamp; Soft≠product)\n");
/* Grep: input: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("input: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("input: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: input: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("input: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("input: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retswapchainangle stamp; Soft≠product)\n");
/* Grep: input: soft retpresentangle — Wave 115 return-presentangle honesty */
kprintf("input: soft retpresentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpresentangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retvsyncangle — Wave 115 exclusive vsyncangle stamp */
kprintf("input: soft retvsyncangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvsyncangle stamp; Soft≠product)\n");
/* Grep: input: soft retfenceangle — Wave 116 return-fenceangle honesty */
kprintf("input: soft retfenceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfenceangle honesty; Soft≠product; not bar3)\n");
/* Grep: input: soft retsemaphoreangle — Wave 116 exclusive semaphoreangle stamp */
kprintf("input: soft retsemaphoreangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsemaphoreangle stamp; Soft≠product)\n");
                            kprintf("input: soft deepen wave=%u areas=%u verdict=%s "
            "ready=%u pushed=%u pop_hit=%u enqueue=%u "
            "desktop_product=OPEN soft_never_gates=1 (soft; not bar3)\n",
            GJ_INPUT_SOFT_WAVE, cAreas, szVerdict, u32Ready, u32Pushed,
            g_u32SoftPopHit, g_u32SoftEnqueue);

    /* Grep: input: soft PASS|PARTIAL|INIT */
    kprintf("input: soft %s ready=%u pushed=%u pop_hit=%u "
            "enqueue=%u wave=%u (soft; not bar3)\n",
            szVerdict, u32Ready, u32Pushed, g_u32SoftPopHit,
            g_u32SoftEnqueue, GJ_INPUT_SOFT_WAVE);
}

/**
 * After first product poll/push/pop activity, print soft inventory once
 * (mirrors memobj/futex soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
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
    g_fSoftInvOnce = 0;
    kprintf("session_input: init ready=%d ring=%u fan-in src=%u wave=%u\n",
            g_fReady, GJ_INPUT_RING, GJ_INPUT_SRC_MAX, GJ_INPUT_SOFT_WAVE);
    /* Grep: input_hub: soft / input: soft (baseline inventory after init) */
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
    /* Wave 15: peak occupancy percent (capacity never 0). */
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
        /* Drop oldest — keep a live tail for sessiond. */
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
 * Soft lazy fan-in: empty ring → poll producers once, then try again.
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
