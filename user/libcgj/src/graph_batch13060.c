/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13060: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13060(void);
 *     - Returns the compile-time graph batch number for this TU (13060).
 *   uint32_t __gj_batch_id_13060  (alias)
 *   __libcgj_batch13060_marker = "libcgj-batch13060"
 *
 * Exclusive continuum CREATE-ONLY (13051-13060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13060 —
 * controller_ok_u_13051, gamepad_ok_u_13052, joystick_ok_u_13053,
 * steam_input_ok_u_13054, hidraw_ok_u_13055, evdev_pad_ok_u_13056,
 * xpad_ok_u_13057, joydev_ok_u_13058, controller_soft_ready_u_13059,
 * batch_id_13060). Unique gj_batch_id_13060 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_12860 (batch12860) and
 * gj_batch_id_12660 (batch12660). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13060_marker[] = "libcgj-batch13060";

#define B13060_BATCH_ID  13060u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13060_id(void)
{
	return B13060_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13060 - report this TU's graph batch number.
 *
 * Always returns 13060. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13060(void)
{
	(void)NULL;
	return b13060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13060(void)
    __attribute__((alias("gj_batch_id_13060")));
