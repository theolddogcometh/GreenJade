/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13460: controller soft all→0 + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13460(void);
 *     - Returns the compile-time graph batch number for this TU (13460).
 *   uint32_t __gj_batch_id_13460  (alias)
 *   __libcgj_batch13460_marker = "libcgj-batch13460"
 *
 * Exclusive continuum CREATE-ONLY (13451-13460: controller soft all→0
 * stubs — controller_ok_u_13451, gamepad_ok_u_13452,
 * steamdeck_controls_ok_u_13453, hid_ok_u_13454, xinput_ok_u_13455,
 * evdev_ok_u_13456, haptics_ok_u_13457, gyro_ok_u_13458,
 * controller_soft_ready_u_13459, batch_id_13460). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13460_marker[] = "libcgj-batch13460";

#define B13460_BATCH_ID  13460u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13460_id(void)
{
	return B13460_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13460 - report this TU's graph batch number.
 *
 * Always returns 13460.
 */
uint32_t
gj_batch_id_13460(void)
{
	(void)NULL;
	return b13460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13460(void)
    __attribute__((alias("gj_batch_id_13460")));
