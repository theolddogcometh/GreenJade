/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7117: clock_gettime clock id validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_id_ok_u_7117(uint32_t clk_id);
 *     - Return 1 if clk_id is a known soft catalog clock id, else 0.
 *   uint32_t __gj_clk_id_ok_u_7117  (alias)
 *   __libcgj_batch7117_marker = "libcgj-batch7117"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_id_ok_u_7117 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7117_marker[] = "libcgj-batch7117";

/*
 * Soft-known Linux clockid values (catalog subset):
 *   REALTIME(0), MONOTONIC(1), PROCESS_CPUTIME(2), THREAD_CPUTIME(3),
 *   MONOTONIC_RAW(4), REALTIME_COARSE(5), MONOTONIC_COARSE(6),
 *   BOOTTIME(7), REALTIME_ALARM(8), BOOTTIME_ALARM(9), TAI(11).
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7117_clk_id_ok(uint32_t u32ClkId)
{
	if (u32ClkId <= 9u)
		return 1u;
	if (u32ClkId == 11u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_id_ok_u_7117 - 1 if clk_id is a known soft catalog value.
 *
 * clk_id: clock_gettime(2) clockid argument (as uint32_t)
 *
 * Returns 1 for ids in {0..9, 11}, else 0. Soft catalog check; does
 * not call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_id_ok_u_7117(uint32_t u32ClkId)
{
	(void)NULL;
	return b7117_clk_id_ok(u32ClkId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_id_ok_u_7117(uint32_t u32ClkId)
    __attribute__((alias("gj_clk_id_ok_u_7117")));
