/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13860: controller soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13860(void);
 *     - Returns the compile-time graph batch number for this TU (13860).
 *   uint32_t __gj_batch_id_13860  (alias)
 *   __libcgj_batch13860_marker = "libcgj-batch13860"
 *
 * Exclusive continuum CREATE-ONLY (13851-13860: controller soft id
 * stubs — controller_ok_u_13851, gamepad_ok_u_13852,
 * steamdeck_controls_ok_u_13853, hid_ok_u_13854, xinput_ok_u_13855,
 * evdev_ok_u_13856, haptics_ok_u_13857, gyro_ok_u_13858,
 * controller_soft_ready_u_13859, batch_id_13860→13860).
 * Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_13660 (batch13660),
 * gj_batch_id_13460 (batch13460), gj_batch_id_13260 (batch13260),
 * gj_batch_id_13060 (batch13060), gj_batch_id_12860 (batch12860) and
 * gj_batch_id_12660 (batch12660). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13860_marker[] = "libcgj-batch13860";

#define B13860_BATCH_ID  13860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13860_id(void)
{
	return B13860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13860 - report this TU's graph batch number.
 *
 * Always returns 13860. Closes the 13851-13860 controller soft wave.
 */
uint32_t
gj_batch_id_13860(void)
{
	(void)NULL;
	return b13860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13860(void)
    __attribute__((alias("gj_batch_id_13860")));
