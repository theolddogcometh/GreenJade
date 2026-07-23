/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6347: DualSense gyro full-scale mdps stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_gyro_fs_mdps_6347(void);
 *     - Return DualSense gyro full-scale in millideg/s (2000000).
 *   uint32_t __gj_ds5_gyro_fs_mdps_6347  (alias)
 *   __libcgj_batch6347_marker = "libcgj-batch6347"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_gyro_fs_mdps_6347 surface only; no
 * multi-def. Distinct from gj_gyro_fs_mdps_5449 (generic IMU). Soft
 * stub — does not sample IMU. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6347_marker[] = "libcgj-batch6347";

/*
 * DualSense gyro full-scale: 2000 deg/s * 1000 = 2000000 millideg/s.
 * Integer-only scaling constant for freestanding IMU graph wiring.
 */
#define B6347_GYRO_FS_MDPS  ((uint32_t)2000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6347_gyro_fs_mdps(void)
{
	return B6347_GYRO_FS_MDPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_gyro_fs_mdps_6347 - DualSense gyro full-scale (2000000 mdps).
 *
 * Always returns 2000000 (2000 deg/s * 1000). Soft compile-time stub;
 * does not open IMU or HID sensor nodes. No parent wires.
 */
uint32_t
gj_ds5_gyro_fs_mdps_6347(void)
{
	(void)NULL;
	return b6347_gyro_fs_mdps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_gyro_fs_mdps_6347(void)
    __attribute__((alias("gj_ds5_gyro_fs_mdps_6347")));
