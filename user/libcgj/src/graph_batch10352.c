/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10352: gamepad ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_10352(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad probe for the controller/input continuum.
 *   uint32_t __gj_gamepad_ok_u_10352  (alias)
 *   __libcgj_batch10352_marker = "libcgj-batch10352"
 *
 * Exclusive continuum CREATE-ONLY (10351-10360: controller/input soft
 * id stubs — controller_ok_u_10351, gamepad_ok_u_10352,
 * steamdeck_controls_ok_u_10353, hid_ok_u_10354, xinput_ok_u_10355,
 * evdev_ok_u_10356, haptics_ok_u_10357, gyro_ok_u_10358,
 * controller_soft_ready_u_10359, batch_id_10360). Unique surface only;
 * no multi-def. Distinct from gj_gamepad_ok_u_10252 (batch10252).
 * No parent wires. No __int128. No gamepad implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10352_marker[] = "libcgj-batch10352";

/* Soft gamepad-ok lamp: always off (not a real gamepad probe). */
#define B10352_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10352_gamepad_ok(void)
{
	return B10352_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_10352 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_10352(void)
{
	(void)NULL;
	return b10352_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_10352(void)
    __attribute__((alias("gj_gamepad_ok_u_10352")));
