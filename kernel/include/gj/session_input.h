/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session input hub — small fan-in ring for virtio-input (A1 path).
 * Pure C11 freestanding, dual MIT OR Apache-2.0.
 *
 * Producers: session_input_poll (idle loop / GJ_SESS_OP_INPUT_POLL) and
 * optional soft inject (session_input_push_from) fan into one ring.
 * Consumers: session_input_pop (GJ_SESS_OP_INPUT_POP / session clients);
 * empty pop soft-refills once from live producers (lazy fan-in).
 *
 * Ring policy (input_hub.c):
 *   depth GJ_INPUT_RING=64; poll burst cap GJ_INPUT_POLL_MAX=256
 *   Full ring: oldest event is dropped (latency over fidelity for the
 *   interim keyboard/pointer path). Drops are counted for STATS.
 *
 * Soft fan-in sources (v1 deepen):
 *   GJ_INPUT_SRC_VIRTIO — live virtio-input drain
 *   GJ_INPUT_SRC_SOFT   — soft inject / secondary producer slot
 * Architecture is multi-source; only virtio is live at A1. Per-source
 * pushed counters support future PS/2 / tablet slots without redesign.
 *
 * Door STATS packing (session_door):
 *   pending count in flags bits 8..15 (clamped 0..255)
 *   sticky drop observed → bit16
 *
 * Greppable:
 *   session_input fan-in
 *   session_input fan-in src
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio_input.h>

/** Soft fan-in source ids (session_input_push_from / poll attribution). */
#define GJ_INPUT_SRC_VIRTIO 0u
#define GJ_INPUT_SRC_SOFT   1u
#define GJ_INPUT_SRC_MAX    2u

/**
 * Reset ring state and sample virtio-input readiness. Safe to re-call
 * (clears pending; counters may soft-reset per implementation).
 */
void session_input_init(void);

/**
 * Drain live producers into the session ring (bounded burst fan-in).
 * Currently source 0 = virtio-input; secondary slots reserved soft.
 * No-op soft when virtio-input is absent (no hard fail).
 */
void session_input_poll(void);

/**
 * Soft fan-in inject: push one event attributed to @u32Src
 * (0 .. GJ_INPUT_SRC_MAX-1). Returns 1 if queued, 0 on bad args.
 * Full ring drops oldest (same policy as poll).
 * greppable: session_input fan-in
 */
int  session_input_push_from(u32 u32Src, const struct gj_input_event *pEv);

/**
 * Pop one event for session clients.
 * Soft lazy fan-in: if the ring is empty, poll producers once then pop.
 * Returns 1 if *pOut filled, 0 if empty or pOut is NULL.
 */
int  session_input_pop(struct gj_input_event *pOut);

/** Lifetime events pushed into the ring (includes later-dropped). */
u32  session_input_pushed(void);

/** Lifetime oldest-dropped count when the ring was full. */
u32  session_input_dropped(void);

/** Events currently queued (0 .. ring depth). */
u32  session_input_pending(void);

/** Soft: peak pending depth observed since init. */
u32  session_input_peak_pending(void);

/** Soft: number of poll bursts that accepted ≥1 event. */
u32  session_input_poll_bursts(void);

/**
 * Soft: events pushed attributed to @u32Src (0 if src out of range).
 * greppable: session_input fan-in src
 */
u32  session_input_pushed_src(u32 u32Src);

/** Non-zero once virtio-input has been observed ready. */
int  session_input_ready(void);
