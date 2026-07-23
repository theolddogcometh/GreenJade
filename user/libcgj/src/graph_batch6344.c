/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6344: DualSense rumble intensity max stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_rumble_max_6344(void);
 *     - Return DualSense dual-motor rumble full-scale intensity (255).
 *   uint32_t __gj_ds5_rumble_max_6344  (alias)
 *   __libcgj_batch6344_marker = "libcgj-batch6344"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_rumble_max_6344 surface only; no
 * multi-def. Distinct from gj_haptic_rumble_max_5448 (16-bit scale).
 * Soft stub — does not drive motors. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6344_marker[] = "libcgj-batch6344";

/* DualSense dual-motor rumble full-scale (8-bit unsigned). */
#define B6344_RUMBLE_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6344_rumble_max(void)
{
	return B6344_RUMBLE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_rumble_max_6344 - DualSense rumble intensity full-scale (255).
 *
 * Always returns 255. Soft compile-time stub for freestanding DualSense
 * motor intensity scaling; does not open FF/haptic devices. No parent
 * wires.
 */
uint32_t
gj_ds5_rumble_max_6344(void)
{
	(void)NULL;
	return b6344_rumble_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_rumble_max_6344(void)
    __attribute__((alias("gj_ds5_rumble_max_6344")));
