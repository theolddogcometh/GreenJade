/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13680: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13680(void);
 *     - Returns the compile-time graph batch number for this TU (13680).
 *   uint32_t __gj_batch_id_13680  (alias)
 *   __libcgj_batch13680_marker = "libcgj-batch13680"
 *
 * Exclusive continuum CREATE-ONLY (13671-13680: power soft all→0 —
 * power_ok_u_13671, tdp_ok_u_13672, battery_ok_u_13673,
 * thermal_ok_u_13674, fan_ok_u_13675, suspend_ok_u_13676,
 * resume_ok_u_13677, performance_ok_u_13678, power_soft_ready_u_13679,
 * batch_id_13680). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13680_marker[] = "libcgj-batch13680";

#define B13680_BATCH_ID  13680u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13680_id(void)
{
	return B13680_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13680 - report this TU's graph batch number.
 *
 * Always returns 13680.
 */
uint32_t
gj_batch_id_13680(void)
{
	(void)NULL;
	return b13680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13680(void)
    __attribute__((alias("gj_batch_id_13680")));
