/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13260: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13260(void);
 *     - Returns the compile-time graph batch number for this TU (13260).
 *   uint32_t __gj_batch_id_13260  (alias)
 *   __libcgj_batch13260_marker = "libcgj-batch13260"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_batch_id_13260 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13060 (batch13060),
 * gj_batch_id_12860 (batch12860) and gj_batch_id_12660 (batch12660).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13260_marker[] = "libcgj-batch13260";

#define B13260_BATCH_ID  13260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13260_id(void)
{
	return B13260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13260 - report this TU's graph batch number.
 *
 * Always returns 13260. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13260(void)
{
	(void)NULL;
	return b13260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13260(void)
    __attribute__((alias("gj_batch_id_13260")));
