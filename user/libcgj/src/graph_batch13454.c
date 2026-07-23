/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13454: HID ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_ok_u_13454(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HID probe for the controller/input continuum.
 *   uint32_t __gj_hid_ok_u_13454  (alias)
 *   __libcgj_batch13454_marker = "libcgj-batch13454"
 *
 * Exclusive continuum CREATE-ONLY (13451-13460: controller soft all→0
 * stubs — controller_ok_u_13451, gamepad_ok_u_13452,
 * steamdeck_controls_ok_u_13453, hid_ok_u_13454, xinput_ok_u_13455,
 * evdev_ok_u_13456, haptics_ok_u_13457, gyro_ok_u_13458,
 * controller_soft_ready_u_13459, batch_id_13460). Unique surface only;
 * no multi-def. No parent wires. No __int128. No HID implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13454_marker[] = "libcgj-batch13454";

/* Soft HID-ok lamp: always off (not a real HID probe). */
#define B13454_HID_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13454_hid_ok(void)
{
	return B13454_HID_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_ok_u_13454 - HID ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open hidraw or
 * call libc. No parent wires.
 */
uint32_t
gj_hid_ok_u_13454(void)
{
	(void)NULL;
	return b13454_hid_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_ok_u_13454(void)
    __attribute__((alias("gj_hid_ok_u_13454")));
