/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4407: floor unsigned 32-bit division
 * (exclusive _u2; rename on collision with batch3003 gj_u32_div_floor_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_div_floor_u2(uint32_t a, uint32_t b);
 *     - floor(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b (including 0).
 *   uint32_t __gj_u32_div_floor_u2  (alias)
 *   __libcgj_batch4407_marker = "libcgj-batch4407"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410). Distinct from
 * gj_u32_div_floor_u (batch3003), gj_u32_fdiv_u (batch3903), and
 * gj_u64_div_floor (batch2034) — unique gj_u32_div_floor_u2 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4407_marker[] = "libcgj-batch4407";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a / b); b == 0 yields 0. */
static uint32_t
b4407_div_floor(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return u32A / u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_div_floor_u2 - floor divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 * No parent wires.
 */
uint32_t
gj_u32_div_floor_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4407_div_floor(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_div_floor_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_div_floor_u2")));
