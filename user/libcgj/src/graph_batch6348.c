/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6348: DualSense accel full-scale mg stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_accel_fs_mg_6348(void);
 *     - Return DualSense accelerometer full-scale in milli-g (8000).
 *   uint32_t __gj_ds5_accel_fs_mg_6348  (alias)
 *   __libcgj_batch6348_marker = "libcgj-batch6348"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_accel_fs_mg_6348 surface only; no
 * multi-def. Soft stub — does not sample IMU. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6348_marker[] = "libcgj-batch6348";

/* DualSense accel full-scale: 8 g * 1000 = 8000 milli-g. */
#define B6348_ACCEL_FS_MG  ((uint32_t)8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6348_accel_fs_mg(void)
{
	return B6348_ACCEL_FS_MG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_accel_fs_mg_6348 - DualSense accel full-scale (8000 mg).
 *
 * Always returns 8000 (8 g * 1000). Soft compile-time stub for
 * freestanding DualSense IMU scaling; does not open sensor nodes.
 * No parent wires.
 */
uint32_t
gj_ds5_accel_fs_mg_6348(void)
{
	(void)NULL;
	return b6348_accel_fs_mg();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_accel_fs_mg_6348(void)
    __attribute__((alias("gj_ds5_accel_fs_mg_6348")));
