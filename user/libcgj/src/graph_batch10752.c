/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10752: gamepad ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_10752(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad probe for the controller soft continuum.
 *   uint32_t __gj_gamepad_ok_u_10752  (alias)
 *   __libcgj_batch10752_marker = "libcgj-batch10752"
 *
 * Exclusive continuum CREATE-ONLY (10751-10760: controller soft all→0
 * stubs — controller_ok_u_10751, gamepad_ok_u_10752,
 * steamdeck_controls_ok_u_10753, hid_ok_u_10754, xinput_ok_u_10755,
 * evdev_ok_u_10756, haptics_ok_u_10757, gyro_ok_u_10758,
 * controller_soft_ready_u_10759, batch_id_10760). Unique surface only;
 * no multi-def. No parent wires. No __int128. No gamepad
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10752_marker[] = "libcgj-batch10752";

/* Soft gamepad-ok lamp: always off (not a real gamepad probe). */
#define B10752_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10752_gamepad_ok(void)
{
	return B10752_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_10752 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_10752(void)
{
	(void)NULL;
	return b10752_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_10752(void)
    __attribute__((alias("gj_gamepad_ok_u_10752")));
