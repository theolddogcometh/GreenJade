/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3902: ceiling unsigned 64-bit division (cdiv_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_cdiv_u(uint64_t a, uint64_t b);
 *     - ceil(a / b) as unsigned integer division.
 *       b == 0 -> 0. a == 0 -> 0 for any b (including 0).
 *       Exact multiples: returns a / b; else (a / b) + 1.
 *   uint64_t __gj_u64_cdiv_u  (alias)
 *   __libcgj_batch3902_marker = "libcgj-batch3902"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910). Distinct from
 * gj_div_ceil_u64 (batch380), gj_u64_div_ceil (batch616),
 * gj_u64_div_ceil_u (batch1067), gj_u64_div_ceil_u_soft (batch2035),
 * gj_u64_div_ceil_u2 (batch2715) — unique gj_u64_cdiv_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3902_marker[] = "libcgj-batch3902";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(a / b) via quotient + (remainder != 0) so intermediate
 * a + b - 1 never overflows UINT64_MAX. b == 0 yields 0.
 */
static uint64_t
b3902_cdiv(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return (u64A / u64B) + ((u64A % u64B) != 0ull ? 1ull : 0ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_cdiv_u - ceiling divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint64_t
gj_u64_cdiv_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3902_cdiv(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_cdiv_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_cdiv_u")));
