/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10356: evdev ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_10356(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Linux evdev probe for the controller/input continuum.
 *   uint32_t __gj_evdev_ok_u_10356  (alias)
 *   __libcgj_batch10356_marker = "libcgj-batch10356"
 *
 * Exclusive continuum CREATE-ONLY (10351-10360: controller/input soft
 * id stubs — controller_ok_u_10351, gamepad_ok_u_10352,
 * steamdeck_controls_ok_u_10353, hid_ok_u_10354, xinput_ok_u_10355,
 * evdev_ok_u_10356, haptics_ok_u_10357, gyro_ok_u_10358,
 * controller_soft_ready_u_10359, batch_id_10360). Unique surface only;
 * no multi-def. Distinct from gj_evdev_ok_u_10256 / gj_evdev_ok_u_10163.
 * No parent wires. No __int128. No evdev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10356_marker[] = "libcgj-batch10356";

/* Soft evdev-ok lamp: always off (not a real /dev/input probe). */
#define B10356_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10356_evdev_ok(void)
{
	return B10356_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_10356 - evdev ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open /dev/input or
 * call libc. No parent wires.
 */
uint32_t
gj_evdev_ok_u_10356(void)
{
	(void)NULL;
	return b10356_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_10356(void)
    __attribute__((alias("gj_evdev_ok_u_10356")));
