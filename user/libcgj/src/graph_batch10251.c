/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10251: controller ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_ok_u_10251(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       controller/input probe for the controller/input continuum.
 *   uint32_t __gj_controller_ok_u_10251  (alias)
 *   __libcgj_batch10251_marker = "libcgj-batch10251"
 *
 * Exclusive continuum CREATE-ONLY (10251-10260: controller/input soft
 * id stubs — controller_ok_u_10251, gamepad_ok_u_10252,
 * steamdeck_controls_ok_u_10253, hid_ok_u_10254, xinput_ok_u_10255,
 * evdev_ok_u_10256, haptics_ok_u_10257, gyro_ok_u_10258,
 * controller_soft_ready_u_10259, batch_id_10260). Unique surface only;
 * no multi-def. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10251_marker[] = "libcgj-batch10251";

/* Soft controller-ok lamp: always off (not a real controller probe). */
#define B10251_CONTROLLER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10251_controller_ok(void)
{
	return B10251_CONTROLLER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_ok_u_10251 - controller ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices or
 * call libc. No parent wires.
 */
uint32_t
gj_controller_ok_u_10251(void)
{
	(void)NULL;
	return b10251_controller_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_ok_u_10251(void)
    __attribute__((alias("gj_controller_ok_u_10251")));
