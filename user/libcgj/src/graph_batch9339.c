/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9339: libinput soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ready_u_9339(void);
 *     - Returns 1 (soft lamp only). Indicates the libinput soft-stub
 *       surfaces in this continuum are present; not device/seat/
 *       tablet/switch readiness.
 *   uint32_t __gj_libinput_ready_u_9339  (alias)
 *   __libcgj_batch9339_marker = "libcgj-batch9339"
 *
 * Exclusive continuum CREATE-ONLY (9331-9340: libinput soft id stubs —
 * li_event_pointer_id_9331, li_event_keyboard_id_9332,
 * li_event_touch_id_9333, li_event_gesture_id_9334,
 * li_device_ok_u_9335, li_seat_ok_u_9336, li_tablet_ok_u_9337,
 * li_switch_ok_u_9338, libinput_ready_u_9339, batch_id_9340).
 * Unique surface only; no multi-def. Device/seat/tablet/switch ok
 * units remain 0. No parent wires. No __int128. No libinput
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9339_marker[] = "libcgj-batch9339";

/* Soft continuum lamp: libinput soft-stub surfaces present. */
#define B9339_LIBINPUT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9339_libinput_ready(void)
{
	return B9339_LIBINPUT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ready_u_9339 - libinput soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 9331-9340 surfaces are present. Does not claim device/seat/tablet/
 * switch ready and does not call libinput/libc. No parent wires.
 */
uint32_t
gj_libinput_ready_u_9339(void)
{
	(void)NULL;
	return b9339_libinput_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ready_u_9339(void)
    __attribute__((alias("gj_libinput_ready_u_9339")));
