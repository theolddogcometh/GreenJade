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
 * Soft input hub inventory (Wave 10 exclusive; this unit only):
 *   - Ring capacity / live pending / peak / free + drop-oldest policy
 *   - Fan-in src tallies (virtio live + soft inject slot)
 *   - Poll burst / push / pop / lazy-refill path counters
 *   Never hard-gates; diagnostics only (wrap OK).
 * Greppable twin prefixes (product / agent greps):
 *   "input_hub: soft …"
 *   "input: soft …"
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
 * Soft product inventory (Wave 10). Cumulative unless noted live/peak.
 * greppable: input_hub: soft … / input: soft …
 */
static u32 g_u32SoftPollEnter;   /* session_input_poll entries */
static u32 g_u32SoftPollNodev;   /* poll with virtio-input not ready */
static u32 g_u32SoftPollDrain;   /* polls that accepted ≥1 event */
static u32 g_u32SoftLastBurst;   /* events drained on last drain poll */
static u32 g_u32SoftBurstMax;    /* peak events in one poll drain */
static u32 g_u32SoftPushOk;      /* session_input_push_from accepted */
static u32 g_u32SoftPushReject;  /* push_from bad args */
static u32 g_u32SoftEnqueue;     /* input_enqueue accepted */
static u32 g_u32SoftDropOld;     /* drop-oldest under full ring (shadow) */
static u32 g_u32SoftPopHit;      /* session_input_pop filled *pOut */
static u32 g_u32SoftPopEmpty;    /* pop terminal empty (after lazy) */
static u32 g_u32SoftPopNull;     /* pop with pOut == NULL */
static u32 g_u32SoftPopLazy;     /* empty-ring lazy fan-in polls */
static u32 g_u32SoftPopLazyHit;  /* lazy refill then successful pop */
static u32 g_u32SoftSaneRepair;  /* input_ring_sane repaired corrupt state */
static u32 g_u32SoftLogN;        /* soft inventory log emissions */
static u8  g_fSoftInvOnce;       /* one-shot deep dump after activity */

static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/**
 * Greppable soft input hub inventory (product / smoke).
 * Twin prefixes so either agent grep works:
 *   input_hub: soft inventory|ring|fan-in|poll|push|pop|path …
 *   input: soft inventory|ring|fan-in|poll|push|pop|path …
 * greppable: input_hub: soft
 * greppable: input: soft
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

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    /* Snapshot live ring (diagnostics only; no hard lock). */
    u32Ready = (g_fReady != 0) ? 1u : 0u;
    if (virtio_input_ready()) {
        u32Ready = 1u;
        g_fReady = 1;
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

    /*
     * Primary prefix: input_hub: soft …
     * Catalog capacity + live ring + fan-in + path tallies for smoke greps.
     */
    /* Grep: input_hub: soft inventory */
    kprintf("input_hub: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "log_n=%u\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped,
            g_u32SoftLogN);

    /* Grep: input_hub: soft ring */
    kprintf("input_hub: soft ring head=%u len=%u free=%u peak=%u "
            "policy=drop_oldest repair=%u\n",
            u32Head, u32Pending, u32Free, u32Peak, g_u32SoftSaneRepair);

    /* Grep: input_hub: soft fan-in */
    kprintf("input_hub: soft fan-in virtio=%u soft=%u src_max=%u "
            "live=virtio inject=soft_src lazy_pop=1\n",
            u32Virtio, u32SoftSrc, GJ_INPUT_SRC_MAX);

    /* Grep: input_hub: soft poll */
    kprintf("input_hub: soft poll enter=%u nodev=%u drain=%u last_burst=%u "
            "burst_max=%u bursts=%u poll_max=%u\n",
            g_u32SoftPollEnter, g_u32SoftPollNodev, g_u32SoftPollDrain,
            g_u32SoftLastBurst, g_u32SoftBurstMax, u32Bursts,
            GJ_INPUT_POLL_MAX);

    /* Grep: input_hub: soft push */
    kprintf("input_hub: soft push ok=%u reject=%u enqueue=%u drop_old=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftEnqueue,
            g_u32SoftDropOld);

    /* Grep: input_hub: soft pop */
    kprintf("input_hub: soft pop hit=%u empty=%u null=%u lazy=%u "
            "lazy_hit=%u\n",
            g_u32SoftPopHit, g_u32SoftPopEmpty, g_u32SoftPopNull,
            g_u32SoftPopLazy, g_u32SoftPopLazyHit);

    /* Grep: input_hub: soft path */
    kprintf("input_hub: soft path claim=virtio+soft_inject irq=0 "
            "drop_oldest=1 lazy_fanin=1 (soft inventory; not bar3)\n");

    /*
     * Twin prefix: input: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: input: soft inventory */
    kprintf("input: soft inventory ring=%u poll_max=%u src_max=%u "
            "ready=%u pending=%u free=%u peak=%u pushed=%u dropped=%u "
            "log_n=%u\n",
            GJ_INPUT_RING, GJ_INPUT_POLL_MAX, GJ_INPUT_SRC_MAX, u32Ready,
            u32Pending, u32Free, u32Peak, u32Pushed, u32Dropped,
            g_u32SoftLogN);

    /* Grep: input: soft ring */
    kprintf("input: soft ring head=%u len=%u free=%u peak=%u "
            "policy=drop_oldest repair=%u\n",
            u32Head, u32Pending, u32Free, u32Peak, g_u32SoftSaneRepair);

    /* Grep: input: soft fan-in */
    kprintf("input: soft fan-in virtio=%u soft=%u src_max=%u "
            "live=virtio inject=soft_src lazy_pop=1\n",
            u32Virtio, u32SoftSrc, GJ_INPUT_SRC_MAX);

    /* Grep: input: soft poll */
    kprintf("input: soft poll enter=%u nodev=%u drain=%u last_burst=%u "
            "burst_max=%u bursts=%u poll_max=%u\n",
            g_u32SoftPollEnter, g_u32SoftPollNodev, g_u32SoftPollDrain,
            g_u32SoftLastBurst, g_u32SoftBurstMax, u32Bursts,
            GJ_INPUT_POLL_MAX);

    /* Grep: input: soft push */
    kprintf("input: soft push ok=%u reject=%u enqueue=%u drop_old=%u\n",
            g_u32SoftPushOk, g_u32SoftPushReject, g_u32SoftEnqueue,
            g_u32SoftDropOld);

    /* Grep: input: soft pop */
    kprintf("input: soft pop hit=%u empty=%u null=%u lazy=%u lazy_hit=%u\n",
            g_u32SoftPopHit, g_u32SoftPopEmpty, g_u32SoftPopNull,
            g_u32SoftPopLazy, g_u32SoftPopLazyHit);

    /* Grep: input: soft path */
    kprintf("input: soft path claim=virtio+soft_inject irq=0 "
            "drop_oldest=1 lazy_fanin=1 (soft inventory; not bar3)\n");
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
        g_u32SoftPopHit == 0 && g_u32SoftPopEmpty == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

/** Reset ring state; safe to call more than once. */
void
session_input_init(void)
{
    memset(g_aRing, 0, sizeof(g_aRing));
    memset(g_aPushedSrc, 0, sizeof(g_aPushedSrc));
    g_u32Head = 0;
    g_u32Len = 0;
    g_u32Pushed = 0;
    g_u32Dropped = 0;
    g_u32PeakPending = 0;
    g_u32PollBursts = 0;
    g_fReady = virtio_input_ready() ? 1 : 0;
    g_u32SoftPollEnter = 0;
    g_u32SoftPollNodev = 0;
    g_u32SoftPollDrain = 0;
    g_u32SoftLastBurst = 0;
    g_u32SoftBurstMax = 0;
    g_u32SoftPushOk = 0;
    g_u32SoftPushReject = 0;
    g_u32SoftEnqueue = 0;
    g_u32SoftDropOld = 0;
    g_u32SoftPopHit = 0;
    g_u32SoftPopEmpty = 0;
    g_u32SoftPopNull = 0;
    g_u32SoftPopLazy = 0;
    g_u32SoftPopLazyHit = 0;
    g_u32SoftSaneRepair = 0;
    g_u32SoftLogN = 0;
    g_fSoftInvOnce = 0;
    kprintf("session_input: init ready=%d ring=%u fan-in src=%u\n", g_fReady,
            GJ_INPUT_RING, GJ_INPUT_SRC_MAX);
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
    if (g_u32Head >= GJ_INPUT_RING || g_u32Len > GJ_INPUT_RING) {
        g_u32Head = 0;
        g_u32Len = 0;
        memset(g_aRing, 0, sizeof(g_aRing));
        g_u32SoftSaneRepair++;
        return 0;
    }
    return 1;
}

static void
input_note_pending(void)
{
    if (g_u32Len > g_u32PeakPending) {
        g_u32PeakPending = g_u32Len;
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
        g_u32Head = (g_u32Head + 1u) % GJ_INPUT_RING;
        g_u32Len--;
        g_u32Dropped++;
        g_u32SoftDropOld++;
    }
    u32Pos = (g_u32Head + g_u32Len) % GJ_INPUT_RING;
    g_aRing[u32Pos] = *pEv;
    g_u32Len++;
    g_u32Pushed++;
    g_aPushedSrc[u32Src]++;
    g_u32SoftEnqueue++;
    input_note_pending();
}

/** Drain virtio-input into the session ring (call from idle / door poll). */
void
session_input_poll(void)
{
    struct gj_input_event ev;
    u32 u32Burst;

    g_u32SoftPollEnter++;

    if (!virtio_input_ready()) {
        g_u32SoftPollNodev++;
        soft_inventory_maybe_once();
        return;
    }
    g_fReady = 1;
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
        g_u32SoftPollDrain++;
        g_u32SoftLastBurst = u32Burst;
        if (u32Burst > g_u32SoftBurstMax) {
            g_u32SoftBurstMax = u32Burst;
        }
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
    if (pEv == NULL || u32Src >= GJ_INPUT_SRC_MAX) {
        g_u32SoftPushReject++;
        soft_inventory_maybe_once();
        return 0;
    }
    (void)input_ring_sane();
    input_enqueue(u32Src, pEv);
    g_u32SoftPushOk++;
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
        g_u32SoftPopNull++;
        soft_inventory_maybe_once();
        return 0;
    }
    if (!input_ring_sane()) {
        g_u32SoftPopEmpty++;
        soft_inventory_maybe_once();
        return 0;
    }
    if (g_u32Len == 0) {
        /* Lazy fan-in soft: refill once before declaring empty. */
        g_u32SoftPopLazy++;
        session_input_poll();
        if (!input_ring_sane() || g_u32Len == 0) {
            g_u32SoftPopEmpty++;
            soft_inventory_maybe_once();
            return 0;
        }
        g_u32SoftPopLazyHit++;
    }
    *pOut = g_aRing[g_u32Head];
    g_u32Head = (g_u32Head + 1u) % GJ_INPUT_RING;
    g_u32Len--;
    g_u32SoftPopHit++;
    soft_inventory_maybe_once();
    return 1;
}

u32
session_input_pushed(void)
{
    return g_u32Pushed;
}

u32
session_input_dropped(void)
{
    return g_u32Dropped;
}

u32
session_input_pending(void)
{
    if (!input_ring_sane()) {
        return 0;
    }
    return g_u32Len;
}

u32
session_input_peak_pending(void)
{
    return g_u32PeakPending;
}

u32
session_input_poll_bursts(void)
{
    return g_u32PollBursts;
}

u32
session_input_pushed_src(u32 u32Src)
{
    if (u32Src >= GJ_INPUT_SRC_MAX) {
        return 0;
    }
    return g_aPushedSrc[u32Src];
}

int
session_input_ready(void)
{
    return g_fReady != 0;
}
