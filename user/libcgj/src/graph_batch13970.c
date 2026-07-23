/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13970: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13970(void);
 *     - Returns the compile-time graph batch number for this TU (13970).
 *   uint32_t __gj_batch_id_13970  (alias)
 *   __libcgj_batch13970_marker = "libcgj-batch13970"
 *
 * Exclusive continuum CREATE-ONLY (13961-13970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13970 —
 * udev_ok_u_13961, libinput_ok_u_13962, evdev_ok_u_13963,
 * uinput_ok_u_13964, hidraw_ok_u_13965, kbd_ok_u_13966,
 * mouse_ok_u_13967, touch_ok_u_13968, input_soft_ready_u_13969,
 * batch_id_13970). Unique gj_batch_id_13970 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13770 (batch13770),
 * gj_batch_id_13570 (batch13570), gj_batch_id_13370 (batch13370), and
 * gj_batch_id_13170 (batch13170). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13970_marker[] = "libcgj-batch13970";

#define B13970_BATCH_ID  13970u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13970_id(void)
{
	return B13970_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13970 - report this TU's graph batch number.
 *
 * Always returns 13970. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 13961-13970 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_13970(void)
{
	(void)NULL;
	return b13970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13970(void)
    __attribute__((alias("gj_batch_id_13970")));
