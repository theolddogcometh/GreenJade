/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13880: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13880(void);
 *     - Returns the compile-time graph batch number for this TU (13880).
 *   uint32_t __gj_batch_id_13880  (alias)
 *   __libcgj_batch13880_marker = "libcgj-batch13880"
 *
 * Exclusive continuum CREATE-ONLY (13871-13880: power soft all→0 —
 * power_ok_u_13871, tdp_ok_u_13872, battery_ok_u_13873,
 * thermal_ok_u_13874, fan_ok_u_13875, suspend_ok_u_13876,
 * resume_ok_u_13877, performance_ok_u_13878, power_soft_ready_u_13879,
 * batch_id_13880). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13880_marker[] = "libcgj-batch13880";

#define B13880_BATCH_ID  13880u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13880_id(void)
{
	return B13880_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13880 - report this TU's graph batch number.
 *
 * Always returns 13880.
 */
uint32_t
gj_batch_id_13880(void)
{
	(void)NULL;
	return b13880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13880(void)
    __attribute__((alias("gj_batch_id_13880")));
