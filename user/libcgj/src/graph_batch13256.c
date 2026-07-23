/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13256: controller soft evdev-pad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_pad_ok_u_13256(void);
 *     - Returns 0 (soft stub: evdev gamepad/pad slot is soft/unprobed;
 *       not a runtime /dev/input/event* or libevdev probe).
 *   uint32_t __gj_evdev_pad_ok_u_13256  (alias)
 *   __libcgj_batch13256_marker = "libcgj-batch13256"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_evdev_pad_ok_u_13256 surface only; no
 * multi-def. Distinct from gj_evdev_pad_ok_u_13056 (batch13056),
 * gj_evdev_pad_ok_u_12856 (batch12856), gj_evdev_pad_ok_u_12656
 * (batch12656), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13256_marker[] = "libcgj-batch13256";

/* Soft-stub evdev pad ok value (unprobed). */
#define B13256_EVDEV_PAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13256_evdev_pad_ok(void)
{
	return B13256_EVDEV_PAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_pad_ok_u_13256 - soft evdev pad ok stub.
 *
 * Always returns 0 (soft/unprobed evdev pad slot). Does not call libc
 * or probe /dev/input/event*. No parent wires.
 */
uint32_t
gj_evdev_pad_ok_u_13256(void)
{
	(void)NULL;
	return b13256_evdev_pad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_pad_ok_u_13256(void)
    __attribute__((alias("gj_evdev_pad_ok_u_13256")));
