/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14360: controller soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14360(void);
 *     - Returns the compile-time graph batch number for this TU (14360).
 *   uint32_t __gj_batch_id_14360  (alias)
 *   __libcgj_batch14360_marker = "libcgj-batch14360"
 *
 * Exclusive continuum CREATE-ONLY (14351-14360: controller soft all→0 —
 * controller_ok_u_14351, gamepad_ok_u_14352,
 * steamdeck_controls_ok_u_14353, hid_ok_u_14354, xinput_ok_u_14355,
 * evdev_ok_u_14356, haptics_ok_u_14357, gyro_ok_u_14358,
 * controller_soft_ready_u_14359, batch_id_14360). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10560 (batch10560). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14360_marker[] = "libcgj-batch14360";

#define B14360_BATCH_ID  14360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14360_id(void)
{
	return B14360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14360 - report this TU's graph batch number.
 *
 * Always returns 14360.
 */
uint32_t
gj_batch_id_14360(void)
{
	(void)NULL;
	return b14360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14360(void)
    __attribute__((alias("gj_batch_id_14360")));
