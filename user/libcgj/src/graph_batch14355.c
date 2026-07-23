/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14355: xinput ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xinput_ok_u_14355(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       XInput / pad runtime probe for the controller soft continuum.
 *   uint32_t __gj_xinput_ok_u_14355  (alias)
 *   __libcgj_batch14355_marker = "libcgj-batch14355"
 *
 * Exclusive continuum CREATE-ONLY (14351-14360: controller soft all→0 —
 * controller_ok_u_14351, gamepad_ok_u_14352,
 * steamdeck_controls_ok_u_14353, hid_ok_u_14354, xinput_ok_u_14355,
 * evdev_ok_u_14356, haptics_ok_u_14357, gyro_ok_u_14358,
 * controller_soft_ready_u_14359, batch_id_14360). Unique surface only;
 * no multi-def. Distinct from gj_xinput_ok_u_10555 (batch10555).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14355_marker[] = "libcgj-batch14355";

/* Soft xinput-ok lamp: always off (not a real XInput probe). */
#define B14355_XINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14355_ok(void)
{
	return B14355_XINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xinput_ok_u_14355 - xinput ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe XInput pad
 * presence or call libc. No parent wires.
 */
uint32_t
gj_xinput_ok_u_14355(void)
{
	(void)NULL;
	return b14355_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xinput_ok_u_14355(void)
    __attribute__((alias("gj_xinput_ok_u_14355")));
