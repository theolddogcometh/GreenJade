/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13480: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13480(void);
 *     - Returns the compile-time graph batch number for this TU (13480).
 *   uint32_t __gj_batch_id_13480  (alias)
 *   __libcgj_batch13480_marker = "libcgj-batch13480"
 *
 * Exclusive continuum CREATE-ONLY (13471-13480: power/thermal soft
 * product stubs — power_ok_u_13471, tdp_ok_u_13472,
 * battery_ok_u_13473, thermal_ok_u_13474, fan_ok_u_13475,
 * suspend_ok_u_13476, resume_ok_u_13477, performance_ok_u_13478,
 * power_soft_ready_u_13479, batch_id_13480). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13480_marker[] = "libcgj-batch13480";

#define B13480_BATCH_ID  13480u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13480_id(void)
{
	return B13480_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13480 - report this TU's graph batch number.
 *
 * Always returns 13480.
 */
uint32_t
gj_batch_id_13480(void)
{
	(void)NULL;
	return b13480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13480(void)
    __attribute__((alias("gj_batch_id_13480")));
