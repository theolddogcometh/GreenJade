/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12860: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12860(void);
 *     - Returns the compile-time graph batch number for this TU (12860).
 *   uint32_t __gj_batch_id_12860  (alias)
 *   __libcgj_batch12860_marker = "libcgj-batch12860"
 *
 * Exclusive continuum CREATE-ONLY (12851-12860: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12860 —
 * controller_ok_u_12851, gamepad_ok_u_12852, joystick_ok_u_12853,
 * steam_input_ok_u_12854, hidraw_ok_u_12855, evdev_pad_ok_u_12856,
 * xpad_ok_u_12857, joydev_ok_u_12858, controller_soft_ready_u_12859,
 * batch_id_12860). Unique gj_batch_id_12860 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_11960 (batch11960) and
 * gj_batch_id_11760 (batch11760). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12860_marker[] = "libcgj-batch12860";

#define B12860_BATCH_ID  12860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12860_id(void)
{
	return B12860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12860 - report this TU's graph batch number.
 *
 * Always returns 12860. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12860(void)
{
	(void)NULL;
	return b12860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12860(void)
    __attribute__((alias("gj_batch_id_12860")));
