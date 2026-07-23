/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10960: controller soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10960(void);
 *     - Returns the compile-time graph batch number for this TU (10960).
 *   uint32_t __gj_batch_id_10960  (alias)
 *   __libcgj_batch10960_marker = "libcgj-batch10960"
 *
 * Exclusive continuum CREATE-ONLY (10951-10960: controller soft id
 * stubs — controller_ok_u_10951, gamepad_ok_u_10952,
 * steamdeck_controls_ok_u_10953, hid_ok_u_10954, xinput_ok_u_10955,
 * evdev_ok_u_10956, haptics_ok_u_10957, gyro_ok_u_10958,
 * controller_soft_ready_u_10959, batch_id_10960). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10960_marker[] = "libcgj-batch10960";

#define B10960_BATCH_ID  10960u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10960_id(void)
{
	return B10960_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10960 - report this TU's graph batch number.
 *
 * Always returns 10960.
 */
uint32_t
gj_batch_id_10960(void)
{
	(void)NULL;
	return b10960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10960(void)
    __attribute__((alias("gj_batch_id_10960")));
