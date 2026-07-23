/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11580: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11580(void);
 *     - Returns the compile-time graph batch number for this TU (11580).
 *   uint32_t __gj_batch_id_11580  (alias)
 *   __libcgj_batch11580_marker = "libcgj-batch11580"
 *
 * Exclusive continuum CREATE-ONLY (11571-11580: power soft all→0 —
 * power_ok_u_11571, tdp_ok_u_11572, battery_ok_u_11573,
 * thermal_ok_u_11574, fan_ok_u_11575, suspend_ok_u_11576,
 * resume_ok_u_11577, performance_ok_u_11578, power_soft_ready_u_11579,
 * batch_id_11580). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11580_marker[] = "libcgj-batch11580";

#define B11580_BATCH_ID  11580u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11580_id(void)
{
	return B11580_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11580 - report this TU's graph batch number.
 *
 * Always returns 11580.
 */
uint32_t
gj_batch_id_11580(void)
{
	(void)NULL;
	return b11580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11580(void)
    __attribute__((alias("gj_batch_id_11580")));
