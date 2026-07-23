/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6346: DualSense touchpad max Y resolution.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_touch_max_y_6346(void);
 *     - Return DualSense capacitive touchpad max Y coordinate (1079).
 *   uint32_t __gj_ds5_touch_max_y_6346  (alias)
 *   __libcgj_batch6346_marker = "libcgj-batch6346"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_touch_max_y_6346 surface only; no
 * multi-def. Distinct from gj_ds5_touch_max_x_6345. Soft stub — does
 * not read touch reports. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6346_marker[] = "libcgj-batch6346";

/* DualSense touchpad vertical max inclusive coordinate. */
#define B6346_TOUCH_MAX_Y  ((uint32_t)1079u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6346_touch_max_y(void)
{
	return B6346_TOUCH_MAX_Y;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_touch_max_y_6346 - DualSense touchpad max Y (1079).
 *
 * Always returns 1079. Soft compile-time constant for freestanding
 * touchpad normalization; does not sample HID touch slots. No parent
 * wires.
 */
uint32_t
gj_ds5_touch_max_y_6346(void)
{
	(void)NULL;
	return b6346_touch_max_y();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_touch_max_y_6346(void)
    __attribute__((alias("gj_ds5_touch_max_y_6346")));
