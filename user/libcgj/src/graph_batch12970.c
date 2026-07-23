/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12970: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12970(void);
 *     - Returns the compile-time graph batch number for this TU (12970).
 *   uint32_t __gj_batch_id_12970  (alias)
 *   __libcgj_batch12970_marker = "libcgj-batch12970"
 *
 * Exclusive continuum CREATE-ONLY (12961-12970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→12970 —
 * udev_ok_u_12961, libinput_ok_u_12962, evdev_ok_u_12963,
 * uinput_ok_u_12964, hidraw_ok_u_12965, kbd_ok_u_12966,
 * mouse_ok_u_12967, touch_ok_u_12968, input_soft_ready_u_12969,
 * batch_id_12970). Unique gj_batch_id_12970 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_12870 (batch12870) and
 * gj_batch_id_12770 (batch12770). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12970_marker[] = "libcgj-batch12970";

#define B12970_BATCH_ID  12970u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12970_id(void)
{
	return B12970_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12970 - report this TU's graph batch number.
 *
 * Always returns 12970. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 12961-12970 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_12970(void)
{
	(void)NULL;
	return b12970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12970(void)
    __attribute__((alias("gj_batch_id_12970")));
