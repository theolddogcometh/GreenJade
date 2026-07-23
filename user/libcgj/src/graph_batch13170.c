/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13170: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13170(void);
 *     - Returns the compile-time graph batch number for this TU (13170).
 *   uint32_t __gj_batch_id_13170  (alias)
 *   __libcgj_batch13170_marker = "libcgj-batch13170"
 *
 * Exclusive continuum CREATE-ONLY (13161-13170: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13170 —
 * udev_ok_u_13161, libinput_ok_u_13162, evdev_ok_u_13163,
 * uinput_ok_u_13164, hidraw_ok_u_13165, kbd_ok_u_13166,
 * mouse_ok_u_13167, touch_ok_u_13168, input_soft_ready_u_13169,
 * batch_id_13170). Unique gj_batch_id_13170 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13070 (batch13070) and
 * gj_batch_id_12970 (batch12970). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13170_marker[] = "libcgj-batch13170";

#define B13170_BATCH_ID  13170u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13170_id(void)
{
	return B13170_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13170 - report this TU's graph batch number.
 *
 * Always returns 13170. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 13161-13170 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_13170(void)
{
	(void)NULL;
	return b13170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13170(void)
    __attribute__((alias("gj_batch_id_13170")));
