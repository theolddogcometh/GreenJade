/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12060: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12060(void);
 *     - Returns the compile-time graph batch number for this TU (12060).
 *   uint32_t __gj_batch_id_12060  (alias)
 *   __libcgj_batch12060_marker = "libcgj-batch12060"
 *
 * Exclusive continuum CREATE-ONLY (12051-12060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12060 —
 * controller_ok_u_12051, gamepad_ok_u_12052, joystick_ok_u_12053,
 * steam_input_ok_u_12054, hidraw_ok_u_12055, evdev_pad_ok_u_12056,
 * xpad_ok_u_12057, joydev_ok_u_12058, controller_soft_ready_u_12059,
 * batch_id_12060). Unique gj_batch_id_12060 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_11960 (batch11960),
 * gj_batch_id_11760 (batch11760), and gj_batch_id_11560 (batch11560).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12060_marker[] = "libcgj-batch12060";

#define B12060_BATCH_ID  12060u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12060_id(void)
{
	return B12060_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12060 - report this TU's graph batch number.
 *
 * Always returns 12060. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12060(void)
{
	(void)NULL;
	return b12060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12060(void)
    __attribute__((alias("gj_batch_id_12060")));
