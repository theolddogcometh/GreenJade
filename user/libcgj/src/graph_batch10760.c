/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10760: controller soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10760(void);
 *     - Returns the compile-time graph batch number for this TU (10760).
 *   uint32_t __gj_batch_id_10760  (alias)
 *   __libcgj_batch10760_marker = "libcgj-batch10760"
 *
 * Exclusive continuum CREATE-ONLY (10751-10760: controller soft all→0
 * stubs — controller_ok_u_10751, gamepad_ok_u_10752,
 * steamdeck_controls_ok_u_10753, hid_ok_u_10754, xinput_ok_u_10755,
 * evdev_ok_u_10756, haptics_ok_u_10757, gyro_ok_u_10758,
 * controller_soft_ready_u_10759, batch_id_10760). Unique surfaces only;
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

const char __libcgj_batch10760_marker[] = "libcgj-batch10760";

#define B10760_BATCH_ID  10760u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10760_id(void)
{
	return B10760_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10760 - report this TU's graph batch number.
 *
 * Always returns 10760.
 */
uint32_t
gj_batch_id_10760(void)
{
	(void)NULL;
	return b10760_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10760(void)
    __attribute__((alias("gj_batch_id_10760")));
