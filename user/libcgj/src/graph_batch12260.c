/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12260: controller/input soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12260(void);
 *     - Returns the compile-time graph batch number for this TU (12260).
 *   uint32_t __gj_batch_id_12260  (alias)
 *   __libcgj_batch12260_marker = "libcgj-batch12260"
 *
 * Exclusive continuum CREATE-ONLY (12251-12260: controller/input soft
 * id stubs — controller_ok_u_12251, gamepad_ok_u_12252,
 * steamdeck_controls_ok_u_12253, hid_ok_u_12254, xinput_ok_u_12255,
 * evdev_ok_u_12256, haptics_ok_u_12257, gyro_ok_u_12258,
 * controller_soft_ready_u_12259, batch_id_12260). Unique surfaces only;
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

const char __libcgj_batch12260_marker[] = "libcgj-batch12260";

#define B12260_BATCH_ID  12260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12260_id(void)
{
	return B12260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12260 - report this TU's graph batch number.
 *
 * Always returns 12260.
 */
uint32_t
gj_batch_id_12260(void)
{
	(void)NULL;
	return b12260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12260(void)
    __attribute__((alias("gj_batch_id_12260")));
