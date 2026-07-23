/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6342: DualSense USB product id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ds5_pid_6342(void);
 *     - Return DualSense USB product id (0x0CE6 == 3302).
 *   uint32_t __gj_ds5_pid_6342  (alias)
 *   __libcgj_batch6342_marker = "libcgj-batch6342"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Unique gj_ds5_pid_6342 surface only; no multi-def.
 * Distinct from gj_ds5_vid_6341. Soft stub — does not open USB/HID.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6342_marker[] = "libcgj-batch6342";

/* DualSense (wireless controller) USB product id. */
#define B6342_DS5_PID  ((uint32_t)0x0CE6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6342_ds5_pid(void)
{
	return B6342_DS5_PID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ds5_pid_6342 - DualSense USB product id (0x0CE6).
 *
 * Always returns 3302 (0x0CE6). Soft compile-time constant for
 * freestanding DualSense match tables; does not enumerate USB.
 * No parent wires.
 */
uint32_t
gj_ds5_pid_6342(void)
{
	(void)NULL;
	return b6342_ds5_pid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ds5_pid_6342(void)
    __attribute__((alias("gj_ds5_pid_6342")));
