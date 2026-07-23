/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9335: libinput device ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_li_device_ok_u_9335(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       libinput device validity probe.
 *   uint32_t __gj_li_device_ok_u_9335  (alias)
 *   __libcgj_batch9335_marker = "libcgj-batch9335"
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

const char __libcgj_batch9335_marker[] = "libcgj-batch9335";

/* Soft libinput device-ok lamp: always off (not a real device probe). */
#define B9335_LI_DEVICE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9335_device_ok(void)
{
	return B9335_LI_DEVICE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_li_device_ok_u_9335 - libinput device ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libinput/libc. No parent wires.
 */
uint32_t
gj_li_device_ok_u_9335(void)
{
	(void)NULL;
	return b9335_device_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_li_device_ok_u_9335(void)
    __attribute__((alias("gj_li_device_ok_u_9335")));
