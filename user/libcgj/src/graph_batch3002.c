/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3002: ceiling base-2 log for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_log2_ceil_u(uint64_t n);
 *     - Smallest k such that 2^k >= n for n >= 1.
 *       n == 0 -> 0 (defined sentinel).
 *       n == 1 -> 0; n == 2 -> 1; n == 3 -> 2; n == 4 -> 2.
 *   unsigned __gj_u64_log2_ceil_u  (alias)
 *   __libcgj_batch3002_marker = "libcgj-batch3002"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010). Distinct from
 * gj_u64_log2_ceil (batch1066) / gj_u64_log2_ceil_soft (batch2244) —
 * unique gj_u64_log2_ceil_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3002_marker[] = "libcgj-batch3002";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(log2(n)) for n > 1 via floor(log2(n - 1)) + 1 binary peel of
 * the highest set bit of (n - 1).
 */
static unsigned
b3002_log2_ceil(uint64_t u64N)
{
	uint64_t u64X;
	unsigned uR;

	u64X = u64N - 1ull;
	uR = 0u;

	if (u64X >= 0x100000000ULL) {
		u64X >>= 32;
		uR += 32u;
	}
	if (u64X >= 0x10000ULL) {
		u64X >>= 16;
		uR += 16u;
	}
	if (u64X >= 0x100ULL) {
		u64X >>= 8;
		uR += 8u;
	}
	if (u64X >= 0x10ULL) {
		u64X >>= 4;
		uR += 4u;
	}
	if (u64X >= 0x4ULL) {
		u64X >>= 2;
		uR += 2u;
	}
	if (u64X >= 0x2ULL) {
		uR += 1u;
	}
	return uR + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_log2_ceil_u - ceil(log2(n)) for n >= 1; 0 when n == 0.
 *
 * Equivalent to floor(log2(n - 1)) + 1 for n > 1. No parent wires.
 */
unsigned
gj_u64_log2_ceil_u(uint64_t u64N)
{
	(void)NULL;
	if (u64N <= 1ull) {
		return 0u;
	}
	return b3002_log2_ceil(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_log2_ceil_u(uint64_t u64N)
    __attribute__((alias("gj_u64_log2_ceil_u")));
