/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10080: power/thermal soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10080(void);
 *     - Returns the compile-time graph batch number for this TU (10080).
 *   uint32_t __gj_batch_id_10080  (alias)
 *   __libcgj_batch10080_marker = "libcgj-batch10080"
 *
 * Exclusive continuum CREATE-ONLY (10071-10080: power/thermal soft id
 * stubs — tdp_ok_u_10071, fan_ok_u_10072, battery_ok_u_10073,
 * thermal_ok_u_10074, cpu_boost_ok_u_10075, gpu_clock_ok_u_10076,
 * suspend_ok_u_10077, resume_ok_u_10078, power_soft_ready_u_10079,
 * batch_id_10080). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10080_marker[] = "libcgj-batch10080";

#define B10080_BATCH_ID  10080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10080_id(void)
{
	return B10080_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10080 - report this TU's graph batch number.
 *
 * Always returns 10080.
 */
uint32_t
gj_batch_id_10080(void)
{
	(void)NULL;
	return b10080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10080(void)
    __attribute__((alias("gj_batch_id_10080")));
