/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10380: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10380(void);
 *     - Returns the compile-time graph batch number for this TU (10380).
 *   uint32_t __gj_batch_id_10380  (alias)
 *   __libcgj_batch10380_marker = "libcgj-batch10380"
 *
 * Exclusive continuum CREATE-ONLY (10371-10380: power soft all→0 —
 * power_ok_u_10371, tdp_ok_u_10372, battery_ok_u_10373,
 * thermal_ok_u_10374, fan_ok_u_10375, suspend_ok_u_10376,
 * resume_ok_u_10377, performance_ok_u_10378, power_soft_ready_u_10379,
 * batch_id_10380). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10380_marker[] = "libcgj-batch10380";

#define B10380_BATCH_ID  10380u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10380_id(void)
{
	return B10380_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10380 - report this TU's graph batch number.
 *
 * Always returns 10380.
 */
uint32_t
gj_batch_id_10380(void)
{
	(void)NULL;
	return b10380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10380(void)
    __attribute__((alias("gj_batch_id_10380")));
