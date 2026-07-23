/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9334: libinput gesture event soft catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_li_event_gesture_id_9334(void);
 *     - Returns 800 (gesture event-class soft catalog value). Pure-data
 *       event-type id stub; does not open devices or call libinput.
 *   uint32_t __gj_li_event_gesture_id_9334  (alias)
 *   __libcgj_batch9334_marker = "libcgj-batch9334"
 *
 * Exclusive continuum CREATE-ONLY (9331-9340: libinput soft id stubs —
 * li_event_pointer_id_9331, li_event_keyboard_id_9332,
 * li_event_touch_id_9333, li_event_gesture_id_9334,
 * li_device_ok_u_9335, li_seat_ok_u_9336, li_tablet_ok_u_9337,
 * li_switch_ok_u_9338, libinput_ready_u_9339, batch_id_9340).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No libinput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9334_marker[] = "libcgj-batch9334";

/* Soft gesture event-class catalog id (800). */
#define B9334_LI_EVENT_GESTURE  800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9334_event_gesture_id(void)
{
	return B9334_LI_EVENT_GESTURE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_li_event_gesture_id_9334 - soft libinput gesture event catalog id.
 *
 * Always returns 800. Soft continuum constant; does not open input
 * devices or call libinput/libc. No parent wires.
 */
uint32_t
gj_li_event_gesture_id_9334(void)
{
	(void)NULL;
	return b9334_event_gesture_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_li_event_gesture_id_9334(void)
    __attribute__((alias("gj_li_event_gesture_id_9334")));
