/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12652: controller soft gamepad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ok_u_12652(void);
 *     - Returns 0 (soft stub: gamepad slot is soft/unprobed; not a
 *       runtime gamepad enumeration or SDL/XInput probe).
 *   uint32_t __gj_gamepad_ok_u_12652  (alias)
 *   __libcgj_batch12652_marker = "libcgj-batch12652"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_gamepad_ok_u_12652 surface only; no
 * multi-def. Distinct from gj_gamepad_ok_u_12452 (batch12452),
 * gj_gamepad_ok_u_12252 (batch12252), gj_gamepad_ok_u_12052
 * (batch12052), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12652_marker[] = "libcgj-batch12652";

/* Soft-stub gamepad ok value (unprobed). */
#define B12652_GAMEPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12652_gamepad_ok(void)
{
	return B12652_GAMEPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ok_u_12652 - soft gamepad ok stub.
 *
 * Always returns 0 (soft/unprobed gamepad slot). Does not call libc or
 * probe gamepad devices. No parent wires.
 */
uint32_t
gj_gamepad_ok_u_12652(void)
{
	(void)NULL;
	return b12652_gamepad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ok_u_12652(void)
    __attribute__((alias("gj_gamepad_ok_u_12652")));
