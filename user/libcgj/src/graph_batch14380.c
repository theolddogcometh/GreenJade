/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14380: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14380(void);
 *     - Returns the compile-time graph batch number for this TU (14380).
 *   uint32_t __gj_batch_id_14380  (alias)
 *   __libcgj_batch14380_marker = "libcgj-batch14380"
 *
 * Exclusive continuum CREATE-ONLY (14371-14380: power soft all→0 —
 * power_ok_u_14371, tdp_ok_u_14372, battery_ok_u_14373,
 * thermal_ok_u_14374, fan_ok_u_14375, suspend_ok_u_14376,
 * resume_ok_u_14377, performance_ok_u_14378, power_soft_ready_u_14379,
 * batch_id_14380). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14380_marker[] = "libcgj-batch14380";

#define B14380_BATCH_ID  14380u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14380_id(void)
{
	return B14380_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14380 - report this TU's graph batch number.
 *
 * Always returns 14380.
 */
uint32_t
gj_batch_id_14380(void)
{
	(void)NULL;
	return b14380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14380(void)
    __attribute__((alias("gj_batch_id_14380")));
