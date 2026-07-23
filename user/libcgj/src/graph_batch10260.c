/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10260: controller/input soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10260(void);
 *     - Returns the compile-time graph batch number for this TU (10260).
 *   uint32_t __gj_batch_id_10260  (alias)
 *   __libcgj_batch10260_marker = "libcgj-batch10260"
 *
 * Exclusive continuum CREATE-ONLY (10251-10260: controller/input soft
 * id stubs — controller_ok_u_10251, gamepad_ok_u_10252,
 * steamdeck_controls_ok_u_10253, hid_ok_u_10254, xinput_ok_u_10255,
 * evdev_ok_u_10256, haptics_ok_u_10257, gyro_ok_u_10258,
 * controller_soft_ready_u_10259, batch_id_10260). Unique surfaces only;
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

const char __libcgj_batch10260_marker[] = "libcgj-batch10260";

#define B10260_BATCH_ID  10260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10260_id(void)
{
	return B10260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10260 - report this TU's graph batch number.
 *
 * Always returns 10260.
 */
uint32_t
gj_batch_id_10260(void)
{
	(void)NULL;
	return b10260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10260(void)
    __attribute__((alias("gj_batch_id_10260")));
