/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13080: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13080(void);
 *     - Returns the compile-time graph batch number for this TU (13080).
 *   uint32_t __gj_batch_id_13080  (alias)
 *   __libcgj_batch13080_marker = "libcgj-batch13080"
 *
 * Exclusive continuum CREATE-ONLY (13071-13080: power/thermal soft
 * product stubs — power_ok_u_13071, tdp_ok_u_13072,
 * battery_ok_u_13073, thermal_ok_u_13074, fan_ok_u_13075,
 * suspend_ok_u_13076, resume_ok_u_13077, performance_ok_u_13078,
 * power_soft_ready_u_13079, batch_id_13080). Unique surfaces only;
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

const char __libcgj_batch13080_marker[] = "libcgj-batch13080";

#define B13080_BATCH_ID  13080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13080_id(void)
{
	return B13080_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13080 - report this TU's graph batch number.
 *
 * Always returns 13080.
 */
uint32_t
gj_batch_id_13080(void)
{
	(void)NULL;
	return b13080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13080(void)
    __attribute__((alias("gj_batch_id_13080")));
