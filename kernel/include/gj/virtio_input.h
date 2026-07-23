/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input event queue (keyboard/tablet/relative) — session
 * input T0. Soft event ring + soft abs/rel axis state (device-side multi-slot
 * drain). Pure C11 freestanding. Dual MIT OR Apache-2.0 only.
 * No Linux virtio source. EV_* / REL_* / ABS_* codes below are public Linux
 * input ABI numbers (userspace protocol), not GPL source.
 *
 * Geometry:
 *   q0 event   — N device-write slots (parallel HW fills; repost on drain)
 *   soft ring  — drop-oldest delivery buffer for poll consumers
 *   soft axes  — last ABS_X/Y + accumulated REL_X/Y/WHEEL
 *   caps       — CFG_EV_BITS at probe and/or observed event types
 *
 * Config selects (OASIS virtio-input; driver-local):
 *   ID_NAME, EV_BITS, ABS_INFO — soft defaults when host omits fields.
 *
 * Consumers: session input hub / compositor / keyboard smoke paths.
 *
 * Bring-up lifecycle:
 *   scan → first KIND_INPUT → setup → negotiate → event q0 multi-slot post
 *   → driver_ok → ready PASS + event ring soft PASS + abs/rel soft PASS
 *
 * Greppable product markers (prefix-stable):
 *   virtio-input: ready PASS
 *   virtio-input: event ring soft PASS
 *   virtio-input: abs/rel axes soft PASS
 *
 * greppable: GJ_EV_ GJ_VIRTIO_INPUT_CAP_ virtio_input_poll
 */
#pragma once

#include <gj/types.h>

/* Public Linux input-event type codes (ABI; not GPL). */
#define GJ_EV_SYN 0u /* sync / report separator */
#define GJ_EV_KEY 1u /* key / button */
#define GJ_EV_REL 2u /* relative axis */
#define GJ_EV_ABS 3u /* absolute axis */

/* Common relative / absolute axis codes (public ABI). */
#define GJ_REL_X     0u
#define GJ_REL_Y     1u
#define GJ_REL_WHEEL 8u
#define GJ_ABS_X     0u
#define GJ_ABS_Y     1u

/*
 * Soft capability bits from CFG_EV_BITS (or observed traffic on drain).
 * Bits may accumulate after probe as events arrive.
 */
#define GJ_VIRTIO_INPUT_CAP_KEY (1u << 0)
#define GJ_VIRTIO_INPUT_CAP_REL (1u << 1)
#define GJ_VIRTIO_INPUT_CAP_ABS (1u << 2)

/**
 * One input event in soft-ring / poll delivery shape.
 * Matches packed virtio-input event field order (type/code/value).
 */
struct gj_input_event {
    u16 u16Type;  /* GJ_EV_* */
    u16 u16Code;  /* keycode or axis code */
    i32 i32Value; /* press/release or axis delta/absolute */
};

/**
 * Soft abs range (from device ABS_INFO when present, else soft defaults).
 * Fuzz/flat/res follow public absinfo meaning; product soft path may zero
 * unused fields when host omits ABS_INFO.
 */
struct gj_input_absinfo {
    i32 i32Min;
    i32 i32Max;
    i32 i32Fuzz;
    i32 i32Flat;
    i32 i32Res;
};

/**
 * Probe first virtio-input; event queue multi-slot post + DRIVER_OK.
 * Returns 0 on success, -1 if no device / setup / negotiate / queue fail.
 */
int  virtio_input_probe(void);

/** Non-zero when DRIVER_OK and event queue slots are live. */
int  virtio_input_ready(void);

/**
 * Poll one event from the soft delivery ring (drains HW slots first into
 * the soft ring, then pops one). Returns 1 if *pOut filled, 0 if none,
 * -1 if not ready / pOut NULL.
 * Non-blocking beyond a short used-ring drain budget.
 */
int  virtio_input_poll(struct gj_input_event *pOut);

/** Lifetime count of events accepted into the soft ring (includes later dropped). */
u32  virtio_input_event_count(void);

/** Events currently buffered in the soft delivery ring (pending poll). */
u32  virtio_input_pending(void);

/**
 * Soft-ring overrun count (oldest dropped when full).
 * Non-zero means consumer is slower than device event rate.
 */
u32  virtio_input_dropped(void);

/**
 * Device-write event slots currently posted on virtqueue 0.
 * Soft refill keeps this near the probe post depth when ready.
 */
u32  virtio_input_slots_posted(void);

/**
 * Soft capability mask (KEY/REL/ABS). Bits may be set from CFG_EV_BITS at
 * probe and/or from observed event types on the drain path.
 * 0 if not ready.
 */
u32  virtio_input_caps(void);

/**
 * Soft relative axes: accumulated REL_X / REL_Y / REL_WHEEL since last reset.
 * Any of the out pointers may be NULL. Returns 0 on success, -1 if not ready.
 * Accumulation is soft (not host-defined); use for pointer smoke / hub.
 */
int  virtio_input_rel_soft(i32 *pX, i32 *pY, i32 *pWheel);

/**
 * Zero soft relative accumulators (absolute state untouched).
 * Safe no-op if not ready.
 */
void virtio_input_rel_reset(void);

/**
 * Soft absolute axes: last ABS_X / ABS_Y values (0 until seen).
 * *pFSeen set non-zero once any ABS event has been applied (may be NULL).
 * Returns 0 on success, -1 if not ready.
 */
int  virtio_input_abs_soft(i32 *pX, i32 *pY, int *pFSeen);

/**
 * Soft ABS_INFO for axis u16Code (GJ_ABS_X / GJ_ABS_Y).
 * Fills *pOut from device config when available, else soft defaults
 * (typically 0..screen or 0..65535 style bring-up range).
 * Returns 0 on success, -1 if not ready / bad arg / unsupported code.
 */
int  virtio_input_absinfo(u16 u16Code, struct gj_input_absinfo *pOut);
