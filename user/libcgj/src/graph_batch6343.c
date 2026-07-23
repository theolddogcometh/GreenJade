/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6343: DualSense adaptive-trigger force max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_at_force_max_6343(void);
 *     - Return adaptive-trigger force full-scale (255).
 *   uint32_t __gj_ds5_at_force_max_6343  (alias)
 *   __libcgj_batch6343_marker = "libcgj-batch6343"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_at_force_max_6343 surface only; no
 * multi-def. Soft stub — does not drive adaptive triggers. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6343_marker[] = "libcgj-batch6343";

/* Adaptive-trigger force full-scale (8-bit unsigned). */
#define B6343_AT_FORCE_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6343_at_force_max(void)
{
	return B6343_AT_FORCE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_at_force_max_6343 - DualSense adaptive-trigger force max (255).
 *
 * Always returns 255. Soft compile-time stub for freestanding L2/R2
 * adaptive-trigger force scaling; does not emit HID output reports.
 * No parent wires.
 */
uint32_t
gj_ds5_at_force_max_6343(void)
{
	(void)NULL;
	return b6343_at_force_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_at_force_max_6343(void)
    __attribute__((alias("gj_ds5_at_force_max_6343")));
