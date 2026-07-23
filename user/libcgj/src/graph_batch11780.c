/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11780: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11780(void);
 *     - Returns the compile-time graph batch number for this TU (11780).
 *   uint32_t __gj_batch_id_11780  (alias)
 *   __libcgj_batch11780_marker = "libcgj-batch11780"
 *
 * Exclusive continuum CREATE-ONLY (11771-11780: power soft all→0 —
 * power_ok_u_11771, tdp_ok_u_11772, battery_ok_u_11773,
 * thermal_ok_u_11774, fan_ok_u_11775, suspend_ok_u_11776,
 * resume_ok_u_11777, performance_ok_u_11778, power_soft_ready_u_11779,
 * batch_id_11780). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11780_marker[] = "libcgj-batch11780";

#define B11780_BATCH_ID  11780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11780_id(void)
{
	return B11780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11780 - report this TU's graph batch number.
 *
 * Always returns 11780.
 */
uint32_t
gj_batch_id_11780(void)
{
	(void)NULL;
	return b11780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11780(void)
    __attribute__((alias("gj_batch_id_11780")));
