/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10780: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10780(void);
 *     - Returns the compile-time graph batch number for this TU (10780).
 *   uint32_t __gj_batch_id_10780  (alias)
 *   __libcgj_batch10780_marker = "libcgj-batch10780"
 *
 * Exclusive continuum CREATE-ONLY (10771-10780: power soft all→0 —
 * power_ok_u_10771, tdp_ok_u_10772, battery_ok_u_10773,
 * thermal_ok_u_10774, fan_ok_u_10775, suspend_ok_u_10776,
 * resume_ok_u_10777, performance_ok_u_10778, power_soft_ready_u_10779,
 * batch_id_10780). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10780_marker[] = "libcgj-batch10780";

#define B10780_BATCH_ID  10780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10780_id(void)
{
	return B10780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10780 - report this TU's graph batch number.
 *
 * Always returns 10780.
 */
uint32_t
gj_batch_id_10780(void)
{
	(void)NULL;
	return b10780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10780(void)
    __attribute__((alias("gj_batch_id_10780")));
