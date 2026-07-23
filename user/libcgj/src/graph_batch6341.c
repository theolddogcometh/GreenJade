/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6341: DualSense USB vendor id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_vid_6341(void);
 *     - Return Sony USB vendor id for DualSense (0x054C == 1356).
 *   uint32_t __gj_ds5_vid_6341  (alias)
 *   __libcgj_batch6341_marker = "libcgj-batch6341"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_vid_6341 surface only; no multi-def.
 * Soft stub — does not open USB/HID devices. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6341_marker[] = "libcgj-batch6341";

/* Sony Interactive Entertainment USB vendor id. */
#define B6341_DS5_VID  ((uint32_t)0x054Cu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6341_ds5_vid(void)
{
	return B6341_DS5_VID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_vid_6341 - DualSense USB vendor id (0x054C).
 *
 * Always returns 1356 (0x054C). Soft compile-time constant for
 * freestanding DualSense graph wiring; does not enumerate USB.
 * No parent wires.
 */
uint32_t
gj_ds5_vid_6341(void)
{
	(void)NULL;
	return b6341_ds5_vid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_vid_6341(void)
    __attribute__((alias("gj_ds5_vid_6341")));
