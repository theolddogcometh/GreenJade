/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11956: controller soft evdev-pad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_pad_ok_u_11956(void);
 *     - Returns 0 (soft stub: evdev gamepad/pad slot is soft/unprobed;
 *       not a runtime /dev/input/event* or libevdev probe).
 *   uint32_t __gj_evdev_pad_ok_u_11956  (alias)
 *   __libcgj_batch11956_marker = "libcgj-batch11956"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_evdev_pad_ok_u_11956 surface only; no
 * multi-def. Distinct from gj_evdev_pad_ok_u_11756 (batch11756),
 * gj_evdev_pad_ok_u_11556 (batch11556), and sibling controller soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11956_marker[] = "libcgj-batch11956";

/* Soft-stub evdev pad ok value (unprobed). */
#define B11956_EVDEV_PAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11956_evdev_pad_ok(void)
{
	return B11956_EVDEV_PAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_pad_ok_u_11956 - soft evdev pad ok stub.
 *
 * Always returns 0 (soft/unprobed evdev pad slot). Does not call libc
 * or probe /dev/input/event*. No parent wires.
 */
uint32_t
gj_evdev_pad_ok_u_11956(void)
{
	(void)NULL;
	return b11956_evdev_pad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_pad_ok_u_11956(void)
    __attribute__((alias("gj_evdev_pad_ok_u_11956")));
