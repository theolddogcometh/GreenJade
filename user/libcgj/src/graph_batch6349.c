/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6349: DualSense lightbar channel max stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_led_max_6349(void);
 *     - Return DualSense lightbar RGB channel full-scale (255).
 *   uint32_t __gj_ds5_led_max_6349  (alias)
 *   __libcgj_batch6349_marker = "libcgj-batch6349"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_led_max_6349 surface only; no multi-def.
 * Soft stub — does not drive lightbar/player LEDs. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6349_marker[] = "libcgj-batch6349";

/* DualSense lightbar RGB channel full-scale (8-bit unsigned). */
#define B6349_LED_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6349_led_max(void)
{
	return B6349_LED_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_led_max_6349 - DualSense lightbar channel max (255).
 *
 * Always returns 255. Soft compile-time stub for freestanding DualSense
 * lightbar RGB scaling; does not emit HID LED reports. No parent wires.
 */
uint32_t
gj_ds5_led_max_6349(void)
{
	(void)NULL;
	return b6349_led_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_led_max_6349(void)
    __attribute__((alias("gj_ds5_led_max_6349")));
