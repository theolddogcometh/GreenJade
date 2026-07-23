/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7119: clock_gettime clock id errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_id_errorish_u_7119(uint32_t clk_id);
 *     - Return 1 if clk_id is outside the known soft catalog, else 0.
 *   uint32_t __gj_clk_id_errorish_u_7119  (alias)
 *   __libcgj_batch7119_marker = "libcgj-batch7119"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_id_errorish_u_7119 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7119_marker[] = "libcgj-batch7119";

/*
 * Soft-known Linux clockid values (same catalog as id_ok):
 *   {0..9, 11}. Anything else is treated as errorish/unknown.
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7119_clk_id_errorish(uint32_t u32ClkId)
{
	if (u32ClkId <= 9u)
		return 0u;
	if (u32ClkId == 11u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_id_errorish_u_7119 - 1 if clk_id is unknown/errorish.
 *
 * clk_id: clock_gettime(2) clockid argument (as uint32_t)
 *
 * Returns 1 when id is outside {0..9, 11}, else 0. Soft inverse of
 * id_ok; does not call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_id_errorish_u_7119(uint32_t u32ClkId)
{
	(void)NULL;
	return b7119_clk_id_errorish(u32ClkId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_id_errorish_u_7119(uint32_t u32ClkId)
    __attribute__((alias("gj_clk_id_errorish_u_7119")));
