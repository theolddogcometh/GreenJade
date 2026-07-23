/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input event queue (keyboard/tablet) — session input T0.
 * Soft event ring + soft abs/rel axis state (device-side multi-slot drain).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-input: ready PASS
 *   virtio-input: event ring soft PASS
 *   virtio-input: abs/rel axes soft PASS
 */
#pragma once

#include <gj/types.h>

/* Public Linux input-event type codes (ABI; not GPL). */
#define GJ_EV_SYN 0u
#define GJ_EV_KEY 1u
#define GJ_EV_REL 2u
#define GJ_EV_ABS 3u

/* Common relative / absolute axis codes (public ABI). */
#define GJ_REL_X     0u
#define GJ_REL_Y     1u
#define GJ_REL_WHEEL 8u
#define GJ_ABS_X     0u
#define GJ_ABS_Y     1u

/* Soft capability bits from CFG_EV_BITS (or observed traffic). */
#define GJ_VIRTIO_INPUT_CAP_KEY (1u << 0)
#define GJ_VIRTIO_INPUT_CAP_REL (1u << 1)
#define GJ_VIRTIO_INPUT_CAP_ABS (1u << 2)

struct gj_input_event {
    u16 u16Type;
    u16 u16Code;
    i32 i32Value;
};

/** Soft abs range (from device ABS_INFO when present, else defaults). */
struct gj_input_absinfo {
    i32 i32Min;
    i32 i32Max;
    i32 i32Fuzz;
    i32 i32Flat;
    i32 i32Res;
};

int  virtio_input_probe(void);
int  virtio_input_ready(void);

/**
 * Poll one event from the soft delivery ring (drains HW slots first).
 * Returns 1 if event filled, 0 if none, -1 if not ready / bad arg.
 */
int  virtio_input_poll(struct gj_input_event *pOut);

/** Lifetime count of events accepted into the soft ring. */
u32  virtio_input_event_count(void);

/** Events currently buffered in the soft delivery ring. */
u32  virtio_input_pending(void);

/** Soft-ring overrun count (oldest dropped when full). */
u32  virtio_input_dropped(void);

/** Device-write event slots currently posted on virtqueue 0. */
u32  virtio_input_slots_posted(void);

/**
 * Soft capability mask (KEY/REL/ABS). Bits may be set from CFG_EV_BITS at
 * probe and/or from observed event types on the drain path.
 */
u32  virtio_input_caps(void);

/**
 * Soft relative axes: accumulated REL_X / REL_Y / REL_WHEEL since last reset.
 * Any of the out pointers may be NULL. Returns 0 on success, -1 if not ready.
 */
int  virtio_input_rel_soft(i32 *pX, i32 *pY, i32 *pWheel);

/** Zero soft relative accumulators (absolute state untouched). */
void virtio_input_rel_reset(void);

/**
 * Soft absolute axes: last ABS_X / ABS_Y values (0 until seen).
 * *pFSeen set non-zero once any ABS event has been applied (may be NULL).
 */
int  virtio_input_abs_soft(i32 *pX, i32 *pY, int *pFSeen);

/**
 * Soft ABS_INFO for axis u16Code (GJ_ABS_X / GJ_ABS_Y).
 * Fills *pOut from device config when available, else soft defaults.
 * Returns 0 on success, -1 if not ready / bad arg / unsupported code.
 */
int  virtio_input_absinfo(u16 u16Code, struct gj_input_absinfo *pOut);
