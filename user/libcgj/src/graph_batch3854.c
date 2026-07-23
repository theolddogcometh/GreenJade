/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3854: floor unsigned 64-bit division (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_floor_div_u(uint64_t a, uint64_t b);
 *     - floor(a / b) as unsigned integer division.
 *       b == 0 -> 0. a == 0 -> 0 for any b (including 0).
 *   uint64_t __gj_u64_floor_div_u  (alias)
 *   __libcgj_batch3854_marker = "libcgj-batch3854"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860). Distinct from
 * gj_u64_div_floor (batch2034), gj_u64_div_floor_u (batch3004), and
 * gj_u32_floor_div_u (batch3853) — unique gj_u64_floor_div_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3854_marker[] = "libcgj-batch3854";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a / b); b == 0 yields 0. */
static uint64_t
b3854_floor_div(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A / u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_floor_div_u - floor divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint64_t
gj_u64_floor_div_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3854_floor_div(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_floor_div_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_floor_div_u")));
