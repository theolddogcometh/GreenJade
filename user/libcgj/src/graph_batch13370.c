/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13370: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13370(void);
 *     - Returns the compile-time graph batch number for this TU (13370).
 *   uint32_t __gj_batch_id_13370  (alias)
 *   __libcgj_batch13370_marker = "libcgj-batch13370"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_batch_id_13370 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13170 (batch13170) and
 * gj_batch_id_12970 (batch12970). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13370_marker[] = "libcgj-batch13370";

#define B13370_BATCH_ID  13370u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13370_id(void)
{
	return B13370_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13370 - report this TU's graph batch number.
 *
 * Always returns 13370. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 13361-13370 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_13370(void)
{
	(void)NULL;
	return b13370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13370(void)
    __attribute__((alias("gj_batch_id_13370")));
