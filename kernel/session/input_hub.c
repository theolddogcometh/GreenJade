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
    kprintf("session_input: init ready=%d ring=%u fan-in src=%u\n", g_fReady,
            GJ_INPUT_RING, GJ_INPUT_SRC_MAX);
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
    }
    u32Pos = (g_u32Head + g_u32Len) % GJ_INPUT_RING;
    g_aRing[u32Pos] = *pEv;
    g_u32Len++;
    g_u32Pushed++;
    g_aPushedSrc[u32Src]++;
    input_note_pending();
}

/** Drain virtio-input into the session ring (call from idle / door poll). */
void
session_input_poll(void)
{
    struct gj_input_event ev;
    u32 u32Burst;

    if (!virtio_input_ready()) {
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
    }
    /*
     * Soft secondary fan-in slot (GJ_INPUT_SRC_SOFT): no live producer at
     * A1; reserved so multi-source architecture is exercised by inject.
     */
}

int
session_input_push_from(u32 u32Src, const struct gj_input_event *pEv)
{
    if (pEv == NULL || u32Src >= GJ_INPUT_SRC_MAX) {
        return 0;
    }
    (void)input_ring_sane();
    input_enqueue(u32Src, pEv);
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
        return 0;
    }
    if (!input_ring_sane()) {
        return 0;
    }
    if (g_u32Len == 0) {
        /* Lazy fan-in soft: refill once before declaring empty. */
        session_input_poll();
        if (!input_ring_sane() || g_u32Len == 0) {
            return 0;
        }
    }
    *pOut = g_aRing[g_u32Head];
    g_u32Head = (g_u32Head + 1u) % GJ_INPUT_RING;
    g_u32Len--;
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
