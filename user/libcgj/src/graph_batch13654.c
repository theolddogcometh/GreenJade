/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13654: HID ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_ok_u_13654(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HID probe for the controller soft continuum.
 *   uint32_t __gj_hid_ok_u_13654  (alias)
 *   __libcgj_batch13654_marker = "libcgj-batch13654"
 *
 * Exclusive continuum CREATE-ONLY (13651-13660: controller soft id
 * stubs — controller_ok_u_13651, gamepad_ok_u_13652,
 * steamdeck_controls_ok_u_13653, hid_ok_u_13654, xinput_ok_u_13655,
 * evdev_ok_u_13656, haptics_ok_u_13657, gyro_ok_u_13658,
 * controller_soft_ready_u_13659, batch_id_13660→13660).
 * Unique surface only; no multi-def. Distinct from
 * gj_hid_ok_u_13454 (batch13454). No parent wires. No __int128. No HID
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13654_marker[] = "libcgj-batch13654";

/* Soft HID-ok lamp: always off (not a real HID probe). */
#define B13654_HID_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13654_hid_ok(void)
{
	return B13654_HID_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_ok_u_13654 - HID ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open hidraw or
 * call libc. No parent wires.
 */
uint32_t
gj_hid_ok_u_13654(void)
{
	(void)NULL;
	return b13654_hid_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_ok_u_13654(void)
    __attribute__((alias("gj_hid_ok_u_13654")));
