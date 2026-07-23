/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12080: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12080(void);
 *     - Returns the compile-time graph batch number for this TU (12080).
 *   uint32_t __gj_batch_id_12080  (alias)
 *   __libcgj_batch12080_marker = "libcgj-batch12080"
 *
 * Exclusive continuum CREATE-ONLY (12071-12080: power soft all→0 —
 * power_ok_u_12071, tdp_ok_u_12072, battery_ok_u_12073,
 * thermal_ok_u_12074, fan_ok_u_12075, suspend_ok_u_12076,
 * resume_ok_u_12077, performance_ok_u_12078, power_soft_ready_u_12079,
 * batch_id_12080). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12080_marker[] = "libcgj-batch12080";

#define B12080_BATCH_ID  12080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12080_id(void)
{
	return B12080_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12080 - report this TU's graph batch number.
 *
 * Always returns 12080.
 */
uint32_t
gj_batch_id_12080(void)
{
	(void)NULL;
	return b12080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12080(void)
    __attribute__((alias("gj_batch_id_12080")));
