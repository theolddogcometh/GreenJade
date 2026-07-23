/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13452: gamepad ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_13452(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad probe for the controller/input continuum.
 *   uint32_t __gj_gamepad_ok_u_13452  (alias)
 *   __libcgj_batch13452_marker = "libcgj-batch13452"
 *
 * Exclusive continuum CREATE-ONLY (13451-13460: controller soft all→0
 * stubs — controller_ok_u_13451, gamepad_ok_u_13452,
 * steamdeck_controls_ok_u_13453, hid_ok_u_13454, xinput_ok_u_13455,
 * evdev_ok_u_13456, haptics_ok_u_13457, gyro_ok_u_13458,
 * controller_soft_ready_u_13459, batch_id_13460). Unique surface only;
 * no multi-def. No parent wires. No __int128. No gamepad
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13452_marker[] = "libcgj-batch13452";

/* Soft gamepad-ok lamp: always off (not a real gamepad probe). */
#define B13452_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13452_gamepad_ok(void)
{
	return B13452_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_13452 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_13452(void)
{
	(void)NULL;
	return b13452_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_13452(void)
    __attribute__((alias("gj_gamepad_ok_u_13452")));
