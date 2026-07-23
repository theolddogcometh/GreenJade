/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11157: haptics ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_haptics_ok_u_11157(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       haptics/rumble probe for the controller soft continuum.
 *   uint32_t __gj_haptics_ok_u_11157  (alias)
 *   __libcgj_batch11157_marker = "libcgj-batch11157"
 *
 * Exclusive continuum CREATE-ONLY (11151-11160: controller soft id
 * stubs — controller_ok_u_11151, gamepad_ok_u_11152,
 * steamdeck_controls_ok_u_11153, hid_ok_u_11154, xinput_ok_u_11155,
 * evdev_ok_u_11156, haptics_ok_u_11157, gyro_ok_u_11158,
 * controller_soft_ready_u_11159, batch_id_11160). Unique surface only;
 * no multi-def. No parent wires. No __int128. No haptics
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11157_marker[] = "libcgj-batch11157";

/* Soft haptics-ok lamp: always off (not a real haptics probe). */
#define B11157_HAPTICS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11157_haptics_ok(void)
{
	return B11157_HAPTICS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_haptics_ok_u_11157 - haptics ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not drive FF/rumble or
 * call libc. No parent wires.
 */
uint32_t
gj_haptics_ok_u_11157(void)
{
	(void)NULL;
	return b11157_haptics_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_haptics_ok_u_11157(void)
    __attribute__((alias("gj_haptics_ok_u_11157")));
