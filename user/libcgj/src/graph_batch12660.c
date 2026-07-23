/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12660: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12660(void);
 *     - Returns the compile-time graph batch number for this TU (12660).
 *   uint32_t __gj_batch_id_12660  (alias)
 *   __libcgj_batch12660_marker = "libcgj-batch12660"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_batch_id_12660 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_12460 (batch12460),
 * gj_batch_id_12260 (batch12260), and gj_batch_id_12060 (batch12060).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12660_marker[] = "libcgj-batch12660";

#define B12660_BATCH_ID  12660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12660_id(void)
{
	return B12660_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12660 - report this TU's graph batch number.
 *
 * Always returns 12660. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12660(void)
{
	(void)NULL;
	return b12660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12660(void)
    __attribute__((alias("gj_batch_id_12660")));
