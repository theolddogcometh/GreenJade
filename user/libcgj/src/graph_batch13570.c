/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13570: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13570(void);
 *     - Returns the compile-time graph batch number for this TU (13570).
 *   uint32_t __gj_batch_id_13570  (alias)
 *   __libcgj_batch13570_marker = "libcgj-batch13570"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_batch_id_13570 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13370 (batch13370) and
 * gj_batch_id_13170 (batch13170). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13570_marker[] = "libcgj-batch13570";

#define B13570_BATCH_ID  13570u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13570_id(void)
{
	return B13570_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13570 - report this TU's graph batch number.
 *
 * Always returns 13570. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 13561-13570 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_13570(void)
{
	(void)NULL;
	return b13570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13570(void)
    __attribute__((alias("gj_batch_id_13570")));
