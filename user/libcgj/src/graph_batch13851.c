/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13851: controller ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_ok_u_13851(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       controller/input probe for the controller soft continuum.
 *   uint32_t __gj_controller_ok_u_13851  (alias)
 *   __libcgj_batch13851_marker = "libcgj-batch13851"
 *
 * Exclusive continuum CREATE-ONLY (13851-13860: controller soft id
 * stubs — controller_ok_u_13851, gamepad_ok_u_13852,
 * steamdeck_controls_ok_u_13853, hid_ok_u_13854, xinput_ok_u_13855,
 * evdev_ok_u_13856, haptics_ok_u_13857, gyro_ok_u_13858,
 * controller_soft_ready_u_13859, batch_id_13860→13860).
 * Unique surface only; no multi-def. Distinct from
 * gj_controller_ok_u_13651 (batch13651), gj_controller_ok_u_13451
 * (batch13451), gj_controller_ok_u_13251 (batch13251),
 * gj_controller_ok_u_13051 (batch13051), gj_controller_ok_u_12851
 * (batch12851) and gj_controller_ok_u_12651 (batch12651).
 * No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13851_marker[] = "libcgj-batch13851";

/* Soft controller-ok lamp: always off (not a real controller probe). */
#define B13851_CONTROLLER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13851_controller_ok(void)
{
	return B13851_CONTROLLER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_ok_u_13851 - controller ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc.
 * No parent wires.
 */
uint32_t
gj_controller_ok_u_13851(void)
{
	(void)NULL;
	return b13851_controller_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_ok_u_13851(void)
    __attribute__((alias("gj_controller_ok_u_13851")));
