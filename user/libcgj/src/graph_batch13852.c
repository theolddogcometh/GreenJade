/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13852: gamepad ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_13852(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad probe for the controller soft continuum.
 *   uint32_t __gj_gamepad_ok_u_13852  (alias)
 *   __libcgj_batch13852_marker = "libcgj-batch13852"
 *
 * Exclusive continuum CREATE-ONLY (13851-13860: controller soft id
 * stubs — controller_ok_u_13851, gamepad_ok_u_13852,
 * steamdeck_controls_ok_u_13853, hid_ok_u_13854, xinput_ok_u_13855,
 * evdev_ok_u_13856, haptics_ok_u_13857, gyro_ok_u_13858,
 * controller_soft_ready_u_13859, batch_id_13860→13860).
 * Unique surface only; no multi-def. Distinct from
 * gj_gamepad_ok_u_13652 (batch13652), gj_gamepad_ok_u_13452
 * (batch13452), gj_gamepad_ok_u_13252 (batch13252),
 * gj_gamepad_ok_u_13052 (batch13052), gj_gamepad_ok_u_12852
 * (batch12852) and gj_gamepad_ok_u_12652 (batch12652).
 * No parent wires. No __int128. No gamepad
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13852_marker[] = "libcgj-batch13852";

/* Soft gamepad-ok lamp: always off (not a real gamepad probe). */
#define B13852_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13852_gamepad_ok(void)
{
	return B13852_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_13852 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc.
 * No parent wires.
 */
uint32_t
gj_gamepad_ok_u_13852(void)
{
	(void)NULL;
	return b13852_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_13852(void)
    __attribute__((alias("gj_gamepad_ok_u_13852")));
