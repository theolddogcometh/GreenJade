/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6345: DualSense touchpad max X resolution.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_touch_max_x_6345(void);
 *     - Return DualSense capacitive touchpad max X coordinate (1919).
 *   uint32_t __gj_ds5_touch_max_x_6345  (alias)
 *   __libcgj_batch6345_marker = "libcgj-batch6345"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_touch_max_x_6345 surface only; no
 * multi-def. Soft stub — does not read touch reports. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6345_marker[] = "libcgj-batch6345";

/* DualSense touchpad horizontal max inclusive coordinate. */
#define B6345_TOUCH_MAX_X  ((uint32_t)1919u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6345_touch_max_x(void)
{
	return B6345_TOUCH_MAX_X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_touch_max_x_6345 - DualSense touchpad max X (1919).
 *
 * Always returns 1919. Soft compile-time constant for freestanding
 * touchpad normalization; does not sample HID touch slots. No parent
 * wires.
 */
uint32_t
gj_ds5_touch_max_x_6345(void)
{
	(void)NULL;
	return b6345_touch_max_x();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_touch_max_x_6345(void)
    __attribute__((alias("gj_ds5_touch_max_x_6345")));
