/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13652: gamepad ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_13652(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad probe for the controller soft continuum.
 *   uint32_t __gj_gamepad_ok_u_13652  (alias)
 *   __libcgj_batch13652_marker = "libcgj-batch13652"
 *
 * Exclusive continuum CREATE-ONLY (13651-13660: controller soft id
 * stubs — controller_ok_u_13651, gamepad_ok_u_13652,
 * steamdeck_controls_ok_u_13653, hid_ok_u_13654, xinput_ok_u_13655,
 * evdev_ok_u_13656, haptics_ok_u_13657, gyro_ok_u_13658,
 * controller_soft_ready_u_13659, batch_id_13660→13660).
 * Unique surface only; no multi-def. Distinct from
 * gj_gamepad_ok_u_13452 (batch13452), gj_gamepad_ok_u_13252
 * (batch13252), gj_gamepad_ok_u_13052 (batch13052),
 * gj_gamepad_ok_u_12852 (batch12852) and gj_gamepad_ok_u_12652
 * (batch12652). No parent wires. No __int128. No gamepad
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13652_marker[] = "libcgj-batch13652";

/* Soft gamepad-ok lamp: always off (not a real gamepad probe). */
#define B13652_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13652_gamepad_ok(void)
{
	return B13652_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_13652 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_13652(void)
{
	(void)NULL;
	return b13652_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_13652(void)
    __attribute__((alias("gj_gamepad_ok_u_13652")));
