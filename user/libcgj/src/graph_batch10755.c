/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10755: XInput ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xinput_ok_u_10755(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XInput probe for the controller soft continuum.
 *   uint32_t __gj_xinput_ok_u_10755  (alias)
 *   __libcgj_batch10755_marker = "libcgj-batch10755"
 *
 * Exclusive continuum CREATE-ONLY (10751-10760: controller soft all→0
 * stubs — controller_ok_u_10751, gamepad_ok_u_10752,
 * steamdeck_controls_ok_u_10753, hid_ok_u_10754, xinput_ok_u_10755,
 * evdev_ok_u_10756, haptics_ok_u_10757, gyro_ok_u_10758,
 * controller_soft_ready_u_10759, batch_id_10760). Unique surface only;
 * no multi-def. No parent wires. No __int128. No XInput
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10755_marker[] = "libcgj-batch10755";

/* Soft XInput-ok lamp: always off (not a real XInput probe). */
#define B10755_XINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10755_xinput_ok(void)
{
	return B10755_XINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xinput_ok_u_10755 - XInput ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open XInput pads or
 * call libc. No parent wires.
 */
uint32_t
gj_xinput_ok_u_10755(void)
{
	(void)NULL;
	return b10755_xinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xinput_ok_u_10755(void)
    __attribute__((alias("gj_xinput_ok_u_10755")));
