/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12460: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12460(void);
 *     - Returns the compile-time graph batch number for this TU (12460).
 *   uint32_t __gj_batch_id_12460  (alias)
 *   __libcgj_batch12460_marker = "libcgj-batch12460"
 *
 * Exclusive continuum CREATE-ONLY (12451-12460: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12460 —
 * controller_ok_u_12451, gamepad_ok_u_12452, joystick_ok_u_12453,
 * steam_input_ok_u_12454, hidraw_ok_u_12455, evdev_pad_ok_u_12456,
 * xpad_ok_u_12457, joydev_ok_u_12458, controller_soft_ready_u_12459,
 * batch_id_12460). Unique gj_batch_id_12460 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_12260 (batch12260),
 * gj_batch_id_12060 (batch12060), and gj_batch_id_11960 (batch11960).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12460_marker[] = "libcgj-batch12460";

#define B12460_BATCH_ID  12460u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12460_id(void)
{
	return B12460_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12460 - report this TU's graph batch number.
 *
 * Always returns 12460. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12460(void)
{
	(void)NULL;
	return b12460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12460(void)
    __attribute__((alias("gj_batch_id_12460")));
