/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13056: controller soft evdev-pad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_pad_ok_u_13056(void);
 *     - Returns 0 (soft stub: evdev gamepad/pad slot is soft/unprobed;
 *       not a runtime /dev/input/event* or libevdev probe).
 *   uint32_t __gj_evdev_pad_ok_u_13056  (alias)
 *   __libcgj_batch13056_marker = "libcgj-batch13056"
 *
 * Exclusive continuum CREATE-ONLY (13051-13060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13060 —
 * controller_ok_u_13051, gamepad_ok_u_13052, joystick_ok_u_13053,
 * steam_input_ok_u_13054, hidraw_ok_u_13055, evdev_pad_ok_u_13056,
 * xpad_ok_u_13057, joydev_ok_u_13058, controller_soft_ready_u_13059,
 * batch_id_13060). Unique gj_evdev_pad_ok_u_13056 surface only; no
 * multi-def. Distinct from gj_evdev_pad_ok_u_12856 (batch12856),
 * gj_evdev_pad_ok_u_12656 (batch12656), and sibling controller soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13056_marker[] = "libcgj-batch13056";

/* Soft-stub evdev pad ok value (unprobed). */
#define B13056_EVDEV_PAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13056_evdev_pad_ok(void)
{
	return B13056_EVDEV_PAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_pad_ok_u_13056 - soft evdev pad ok stub.
 *
 * Always returns 0 (soft/unprobed evdev pad slot). Does not call libc
 * or probe /dev/input/event*. No parent wires.
 */
uint32_t
gj_evdev_pad_ok_u_13056(void)
{
	(void)NULL;
	return b13056_evdev_pad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_pad_ok_u_13056(void)
    __attribute__((alias("gj_evdev_pad_ok_u_13056")));
