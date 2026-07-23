/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3904: floor unsigned 64-bit division (fdiv_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_fdiv_u(uint64_t a, uint64_t b);
 *     - floor(a / b) as unsigned integer division.
 *       b == 0 -> 0. a == 0 -> 0 for any b (including 0).
 *   uint64_t __gj_u64_fdiv_u  (alias)
 *   __libcgj_batch3904_marker = "libcgj-batch3904"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910). Distinct from
 * gj_u64_div_floor_u (batch3004) — unique gj_u64_fdiv_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3904_marker[] = "libcgj-batch3904";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a / b); b == 0 yields 0. */
static uint64_t
b3904_fdiv(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A / u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_fdiv_u - floor divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint64_t
gj_u64_fdiv_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3904_fdiv(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_fdiv_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_fdiv_u")));
