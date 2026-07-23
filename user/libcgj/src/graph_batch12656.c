/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12656: controller soft evdev pad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_pad_ok_u_12656(void);
 *     - Returns 0 (soft stub: Linux evdev pad slot is soft/unprobed;
 *       not a runtime /dev/input/event* open or EVIOC* probe).
 *   uint32_t __gj_evdev_pad_ok_u_12656  (alias)
 *   __libcgj_batch12656_marker = "libcgj-batch12656"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_evdev_pad_ok_u_12656 surface only; no
 * multi-def. Distinct from gj_evdev_pad_ok_u_12456 (batch12456),
 * gj_evdev_pad_ok_u_12056 (batch12056), gj_evdev_pad_ok_u_11956
 * (batch11956), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12656_marker[] = "libcgj-batch12656";

/* Soft-stub evdev pad ok value (unprobed). */
#define B12656_EVDEV_PAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12656_evdev_pad_ok(void)
{
	return B12656_EVDEV_PAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_pad_ok_u_12656 - soft evdev pad ok stub.
 *
 * Always returns 0 (soft/unprobed evdev pad slot). Does not call libc
 * or probe /dev/input/event*. No parent wires.
 */
uint32_t
gj_evdev_pad_ok_u_12656(void)
{
	(void)NULL;
	return b12656_evdev_pad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_pad_ok_u_12656(void)
    __attribute__((alias("gj_evdev_pad_ok_u_12656")));
