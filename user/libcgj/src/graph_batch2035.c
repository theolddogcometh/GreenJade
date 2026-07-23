/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2035: soft ceiling unsigned 64-bit division.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_ceil_u_soft(uint64_t a, uint64_t b);
 *     — ceil(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b.
 *   uint64_t __gj_u64_div_ceil_u_soft  (alias)
 *   __libcgj_batch2035_marker = "libcgj-batch2035"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Renamed from the
 * planned gj_u64_div_ceil_u because that symbol is owned by batch1067;
 * _soft suffix keeps the surface unique (no multi-def). Also distinct
 * from gj_u64_div_ceil (batch616) and gj_div_ceil_u64 (batch380).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2035_marker[] = "libcgj-batch2035";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(a / b) via quotient + (remainder != 0) so intermediate
 * a + b - 1 never overflows UINT64_MAX. b == 0 yields 0.
 */
static uint64_t
b2035_div_ceil(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return (u64A / u64B) + ((u64A % u64B) != 0ull ? 1ull : 0ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_ceil_u_soft — ceiling divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 */
uint64_t
gj_u64_div_ceil_u_soft(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2035_div_ceil(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_ceil_u_soft(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_div_ceil_u_soft")));
