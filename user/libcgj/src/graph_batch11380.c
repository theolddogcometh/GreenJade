/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11380: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11380(void);
 *     - Returns the compile-time graph batch number for this TU (11380).
 *   uint32_t __gj_batch_id_11380  (alias)
 *   __libcgj_batch11380_marker = "libcgj-batch11380"
 *
 * Exclusive continuum CREATE-ONLY (11371-11380: power soft all→0 —
 * power_ok_u_11371, tdp_ok_u_11372, battery_ok_u_11373,
 * thermal_ok_u_11374, fan_ok_u_11375, suspend_ok_u_11376,
 * resume_ok_u_11377, performance_ok_u_11378, power_soft_ready_u_11379,
 * batch_id_11380). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11380_marker[] = "libcgj-batch11380";

#define B11380_BATCH_ID  11380u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11380_id(void)
{
	return B11380_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11380 - report this TU's graph batch number.
 *
 * Always returns 11380.
 */
uint32_t
gj_batch_id_11380(void)
{
	(void)NULL;
	return b11380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11380(void)
    __attribute__((alias("gj_batch_id_11380")));
