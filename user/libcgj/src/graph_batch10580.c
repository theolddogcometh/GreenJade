/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10580: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10580(void);
 *     - Returns the compile-time graph batch number for this TU (10580).
 *   uint32_t __gj_batch_id_10580  (alias)
 *   __libcgj_batch10580_marker = "libcgj-batch10580"
 *
 * Exclusive continuum CREATE-ONLY (10571-10580: power soft all→0 —
 * power_ok_u_10571, tdp_ok_u_10572, battery_ok_u_10573,
 * thermal_ok_u_10574, fan_ok_u_10575, suspend_ok_u_10576,
 * resume_ok_u_10577, performance_ok_u_10578, power_soft_ready_u_10579,
 * batch_id_10580). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10080 (batch10080).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10580_marker[] = "libcgj-batch10580";

#define B10580_BATCH_ID  10580u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10580_id(void)
{
	return B10580_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10580 - report this TU's graph batch number.
 *
 * Always returns 10580.
 */
uint32_t
gj_batch_id_10580(void)
{
	(void)NULL;
	return b10580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10580(void)
    __attribute__((alias("gj_batch_id_10580")));
