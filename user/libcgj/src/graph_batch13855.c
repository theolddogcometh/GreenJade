/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13855: XInput ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xinput_ok_u_13855(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XInput probe for the controller soft continuum.
 *   uint32_t __gj_xinput_ok_u_13855  (alias)
 *   __libcgj_batch13855_marker = "libcgj-batch13855"
 *
 * Exclusive continuum CREATE-ONLY (13851-13860: controller soft id
 * stubs — controller_ok_u_13851, gamepad_ok_u_13852,
 * steamdeck_controls_ok_u_13853, hid_ok_u_13854, xinput_ok_u_13855,
 * evdev_ok_u_13856, haptics_ok_u_13857, gyro_ok_u_13858,
 * controller_soft_ready_u_13859, batch_id_13860→13860).
 * Unique surface only; no multi-def. Distinct from
 * gj_xinput_ok_u_13655 (batch13655), gj_xinput_ok_u_13455
 * (batch13455).
 * No parent wires. No __int128. No
 * XInput implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13855_marker[] = "libcgj-batch13855";

/* Soft XInput-ok lamp: always off (not a real XInput probe). */
#define B13855_XINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13855_xinput_ok(void)
{
	return B13855_XINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xinput_ok_u_13855 - XInput ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open XInput paths or
 * call libc.
 * No parent wires.
 */
uint32_t
gj_xinput_ok_u_13855(void)
{
	(void)NULL;
	return b13855_xinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xinput_ok_u_13855(void)
    __attribute__((alias("gj_xinput_ok_u_13855")));
