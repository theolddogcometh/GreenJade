/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11952: controller soft gamepad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_11952(void);
 *     - Returns 0 (soft stub: gamepad slot is soft/unprobed; not a
 *       runtime gamepad enumeration or SDL/XInput probe).
 *   uint32_t __gj_gamepad_ok_u_11952  (alias)
 *   __libcgj_batch11952_marker = "libcgj-batch11952"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_gamepad_ok_u_11952 surface only; no
 * multi-def. Distinct from gj_gamepad_ok_u_11752 (batch11752),
 * gj_gamepad_ok_u_11552 (batch11552), and sibling controller soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11952_marker[] = "libcgj-batch11952";

/* Soft-stub gamepad ok value (unprobed). */
#define B11952_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11952_gamepad_ok(void)
{
	return B11952_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_11952 - soft gamepad ok stub.
 *
 * Always returns 0 (soft/unprobed gamepad slot). Does not call libc or
 * probe gamepad devices. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_11952(void)
{
	(void)NULL;
	return b11952_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_11952(void)
    __attribute__((alias("gj_gamepad_ok_u_11952")));
