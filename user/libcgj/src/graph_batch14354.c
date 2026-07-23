/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14354: hid ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_ok_u_14354(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HID device runtime probe for the controller soft continuum.
 *   uint32_t __gj_hid_ok_u_14354  (alias)
 *   __libcgj_batch14354_marker = "libcgj-batch14354"
 *
 * Exclusive continuum CREATE-ONLY (14351-14360: controller soft all→0 —
 * controller_ok_u_14351, gamepad_ok_u_14352,
 * steamdeck_controls_ok_u_14353, hid_ok_u_14354, xinput_ok_u_14355,
 * evdev_ok_u_14356, haptics_ok_u_14357, gyro_ok_u_14358,
 * controller_soft_ready_u_14359, batch_id_14360). Unique surface only;
 * no multi-def. Distinct from gj_hid_ok_u_10554 (batch10554).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14354_marker[] = "libcgj-batch14354";

/* Soft hid-ok lamp: always off (not a real HID probe). */
#define B14354_HID_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14354_ok(void)
{
	return B14354_HID_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_ok_u_14354 - hid ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open HID nodes or
 * call libc. No parent wires.
 */
uint32_t
gj_hid_ok_u_14354(void)
{
	(void)NULL;
	return b14354_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_ok_u_14354(void)
    __attribute__((alias("gj_hid_ok_u_14354")));
