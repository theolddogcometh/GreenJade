/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6350: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6350(void);
 *     - Returns the compile-time graph batch number for this TU (6350).
 *   uint32_t __gj_batch_id_6350  (alias)
 *   __libcgj_batch6350_marker = "libcgj-batch6350"
 *
 * Exclusive continuum CREATE-ONLY (6341-6350: dual-sense controller
 * feature stubs — ds5_vid_6341, ds5_pid_6342, ds5_at_force_max_6343,
 * ds5_rumble_max_6344, ds5_touch_max_x_6345, ds5_touch_max_y_6346,
 * ds5_gyro_fs_mdps_6347, ds5_accel_fs_mg_6348, ds5_led_max_6349,
 * batch_id_6350). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6350_marker[] = "libcgj-batch6350";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6350_id(void)
{
	return 6350u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6350 - report this TU's graph batch number.
 *
 * Always returns 6350. Distinct from earlier gj_batch_id /
 * gj_batch_id_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_batch_id_6350(void)
{
	(void)NULL;
	return b6350_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6350(void)
    __attribute__((alias("gj_batch_id_6350")));
