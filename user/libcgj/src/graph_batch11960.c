/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11960: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11960(void);
 *     - Returns the compile-time graph batch number for this TU (11960).
 *   uint32_t __gj_batch_id_11960  (alias)
 *   __libcgj_batch11960_marker = "libcgj-batch11960"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_batch_id_11960 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_11760 (batch11760) and
 * gj_batch_id_11560 (batch11560). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11960_marker[] = "libcgj-batch11960";

#define B11960_BATCH_ID  11960u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11960_id(void)
{
	return B11960_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11960 - report this TU's graph batch number.
 *
 * Always returns 11960. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11960(void)
{
	(void)NULL;
	return b11960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11960(void)
    __attribute__((alias("gj_batch_id_11960")));
