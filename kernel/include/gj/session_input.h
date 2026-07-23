/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session input hub — small fan-out ring for virtio-input (A1 path).
 *
 * Producers: session_input_poll (idle loop / GJ_SESS_OP_INPUT_POLL).
 * Consumers: session_input_pop (GJ_SESS_OP_INPUT_POP / session clients).
 *
 * Full ring: oldest event is dropped (latency over fidelity for the
 * interim keyboard/pointer path). Drops are counted for STATS/observability.
 */
#pragma once

#include <gj/types.h>
#include <gj/virtio_input.h>

/** Reset ring state and sample virtio-input readiness. Safe to re-call. */
void session_input_init(void);

/** Drain virtio-input into the session ring (bounded burst). */
void session_input_poll(void);

/**
 * Pop one event for session clients.
 * Returns 1 if *pOut filled, 0 if empty or pOut is NULL.
 */
int  session_input_pop(struct gj_input_event *pOut);

/** Lifetime events pushed into the ring (includes later-dropped). */
u32  session_input_pushed(void);

/** Lifetime oldest-dropped count when the ring was full. */
u32  session_input_dropped(void);

/** Events currently queued (0 .. ring depth). */
u32  session_input_pending(void);

/** Non-zero once virtio-input has been observed ready. */
int  session_input_ready(void);
