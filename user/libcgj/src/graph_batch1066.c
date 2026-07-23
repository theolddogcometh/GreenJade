/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1066: ceiling base-2 log for uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_log2_ceil(uint64_t n);
 *     — Smallest k such that 2^k >= n for n >= 1.
 *       n == 0 → 0 (defined sentinel).
 *       n == 1 → 0; n == 2 → 1; n == 3 → 2; n == 4 → 2.
 *   unsigned __gj_u64_log2_ceil  (alias)
 *   __libcgj_batch1066_marker = "libcgj-batch1066"
 *
 * Distinct from gj_ilog2_u64 (batch372 floor) / gj_floor_log2_u64
 * (batch579) / gj_bitwidth_u64 (batch591) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1066_marker[] = "libcgj-batch1066";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_log2_ceil — ceil(log2(n)) for n >= 1; 0 when n == 0.
 *
 * Equivalent to floor(log2(n - 1)) + 1 for n > 1, via a binary peel of
 * the highest set bit of (n - 1).
 */
unsigned
gj_u64_log2_ceil(uint64_t u64N)
{
	uint64_t u64X;
	unsigned uR;

	if (u64N <= 1ull) {
		return 0u;
	}

	/* floor(log2(n - 1)) + 1 == ceil(log2(n)) for n > 1 */
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

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_log2_ceil(uint64_t u64N)
    __attribute__((alias("gj_u64_log2_ceil")));
