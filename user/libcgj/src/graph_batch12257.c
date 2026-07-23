/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12257: haptics ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_haptics_ok_u_12257(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       haptics/rumble probe for the controller/input continuum.
 *   uint32_t __gj_haptics_ok_u_12257  (alias)
 *   __libcgj_batch12257_marker = "libcgj-batch12257"
 *
 * Exclusive continuum CREATE-ONLY (12251-12260: controller/input soft
 * id stubs — controller_ok_u_12251, gamepad_ok_u_12252,
 * steamdeck_controls_ok_u_12253, hid_ok_u_12254, xinput_ok_u_12255,
 * evdev_ok_u_12256, haptics_ok_u_12257, gyro_ok_u_12258,
 * controller_soft_ready_u_12259, batch_id_12260). Unique surface only;
 * no multi-def. No parent wires. No __int128. No haptics
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12257_marker[] = "libcgj-batch12257";

/* Soft haptics-ok lamp: always off (not a real haptics probe). */
#define B12257_HAPTICS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12257_haptics_ok(void)
{
	return B12257_HAPTICS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_haptics_ok_u_12257 - haptics ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not drive FF/rumble or
 * call libc. No parent wires.
 */
uint32_t
gj_haptics_ok_u_12257(void)
{
	(void)NULL;
	return b12257_haptics_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_haptics_ok_u_12257(void)
    __attribute__((alias("gj_haptics_ok_u_12257")));
