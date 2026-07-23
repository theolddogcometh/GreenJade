/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10360: controller/input soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10360(void);
 *     - Returns the compile-time graph batch number for this TU (10360).
 *   uint32_t __gj_batch_id_10360  (alias)
 *   __libcgj_batch10360_marker = "libcgj-batch10360"
 *
 * Exclusive continuum CREATE-ONLY (10351-10360: controller/input soft
 * id stubs — controller_ok_u_10351, gamepad_ok_u_10352,
 * steamdeck_controls_ok_u_10353, hid_ok_u_10354, xinput_ok_u_10355,
 * evdev_ok_u_10356, haptics_ok_u_10357, gyro_ok_u_10358,
 * controller_soft_ready_u_10359, batch_id_10360). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10260. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10360_marker[] = "libcgj-batch10360";

#define B10360_BATCH_ID  10360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10360_id(void)
{
	return B10360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10360 - report this TU's graph batch number.
 *
 * Always returns 10360.
 */
uint32_t
gj_batch_id_10360(void)
{
	(void)NULL;
	return b10360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10360(void)
    __attribute__((alias("gj_batch_id_10360")));
