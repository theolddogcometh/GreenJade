/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input event queue (keyboard/tablet) — session input T0.
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-input: ready PASS
 */
#pragma once

#include <gj/types.h>

struct gj_input_event {
    u16 u16Type;
    u16 u16Code;
    i32 i32Value;
};

int  virtio_input_probe(void);
int  virtio_input_ready(void);

/**
 * Poll one event; returns 1 if event filled, 0 if none, -1 if not ready.
 */
int  virtio_input_poll(struct gj_input_event *pOut);

u32  virtio_input_event_count(void);
