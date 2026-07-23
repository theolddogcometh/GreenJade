/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13770: exclusive input soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13770(void);
 *     - Returns the compile-time graph batch number for this TU (13770).
 *   uint32_t __gj_batch_id_13770  (alias)
 *   __libcgj_batch13770_marker = "libcgj-batch13770"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_batch_id_13770 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13570 (batch13570),
 * gj_batch_id_13370 (batch13370), and gj_batch_id_13170 (batch13170).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13770_marker[] = "libcgj-batch13770";

#define B13770_BATCH_ID  13770u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13770_id(void)
{
	return B13770_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13770 - report this TU's graph batch number.
 *
 * Always returns 13770. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * Closes the 13761-13770 input soft wave. No parent wires.
 */
uint32_t
gj_batch_id_13770(void)
{
	(void)NULL;
	return b13770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13770(void)
    __attribute__((alias("gj_batch_id_13770")));
