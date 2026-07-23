/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7118: clock_gettime monotonic-class id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_id_is_monotonic_u_7118(uint32_t clk_id);
 *     - Return 1 if clk_id is a monotonic-class catalog id, else 0.
 *   uint32_t __gj_clk_id_is_monotonic_u_7118  (alias)
 *   __libcgj_batch7118_marker = "libcgj-batch7118"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_id_is_monotonic_u_7118 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7118_marker[] = "libcgj-batch7118";

/*
 * Monotonic-class catalog ids:
 *   CLOCK_MONOTONIC (1), CLOCK_MONOTONIC_RAW (4),
 *   CLOCK_MONOTONIC_COARSE (6), CLOCK_BOOTTIME (7),
 *   CLOCK_BOOTTIME_ALARM (9).
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7118_clk_id_is_monotonic(uint32_t u32ClkId)
{
	if (u32ClkId == 1u)
		return 1u;
	if (u32ClkId == 4u)
		return 1u;
	if (u32ClkId == 6u)
		return 1u;
	if (u32ClkId == 7u)
		return 1u;
	if (u32ClkId == 9u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_id_is_monotonic_u_7118 - 1 if clk_id is monotonic-class.
 *
 * clk_id: clock_gettime(2) clockid argument (as uint32_t)
 *
 * Returns 1 for ids in {1, 4, 6, 7, 9}, else 0. Soft class test; does
 * not call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_id_is_monotonic_u_7118(uint32_t u32ClkId)
{
	(void)NULL;
	return b7118_clk_id_is_monotonic(u32ClkId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_id_is_monotonic_u_7118(uint32_t u32ClkId)
    __attribute__((alias("gj_clk_id_is_monotonic_u_7118")));
