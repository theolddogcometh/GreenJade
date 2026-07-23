/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10552: gamepad ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_10552(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       gamepad runtime probe for the controller soft continuum.
 *   uint32_t __gj_gamepad_ok_u_10552  (alias)
 *   __libcgj_batch10552_marker = "libcgj-batch10552"
 *
 * Exclusive continuum CREATE-ONLY (10551-10560: controller soft all→0 —
 * controller_ok_u_10551, gamepad_ok_u_10552,
 * steamdeck_controls_ok_u_10553, hid_ok_u_10554, xinput_ok_u_10555,
 * evdev_ok_u_10556, haptics_ok_u_10557, gyro_ok_u_10558,
 * controller_soft_ready_u_10559, batch_id_10560). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10552_marker[] = "libcgj-batch10552";

/* Soft gamepad-ok lamp: always off (not a real pad probe). */
#define B10552_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10552_ok(void)
{
	return B10552_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_10552 - gamepad ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe gamepad
 * presence or call libc. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_10552(void)
{
	(void)NULL;
	return b10552_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_10552(void)
    __attribute__((alias("gj_gamepad_ok_u_10552")));
