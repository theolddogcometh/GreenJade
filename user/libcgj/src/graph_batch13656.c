/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13656: evdev ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_13656(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Linux evdev probe for the controller soft continuum.
 *   uint32_t __gj_evdev_ok_u_13656  (alias)
 *   __libcgj_batch13656_marker = "libcgj-batch13656"
 *
 * Exclusive continuum CREATE-ONLY (13651-13660: controller soft id
 * stubs — controller_ok_u_13651, gamepad_ok_u_13652,
 * steamdeck_controls_ok_u_13653, hid_ok_u_13654, xinput_ok_u_13655,
 * evdev_ok_u_13656, haptics_ok_u_13657, gyro_ok_u_13658,
 * controller_soft_ready_u_13659, batch_id_13660→13660).
 * Unique surface only; no multi-def. Distinct from
 * gj_evdev_ok_u_13456 (batch13456). No parent wires. No __int128. No
 * evdev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13656_marker[] = "libcgj-batch13656";

/* Soft evdev-ok lamp: always off (not a real /dev/input probe). */
#define B13656_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13656_evdev_ok(void)
{
	return B13656_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_13656 - evdev ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open /dev/input or
 * call libc. No parent wires.
 */
uint32_t
gj_evdev_ok_u_13656(void)
{
	(void)NULL;
	return b13656_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_13656(void)
    __attribute__((alias("gj_evdev_ok_u_13656")));
