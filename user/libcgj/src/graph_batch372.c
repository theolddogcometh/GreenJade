/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch372: floor base-2 logarithm for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ilog2_u64(uint64_t n);
 *     — Floor of log2(n) for n >= 1 (position of the highest set bit,
 *       0-based: gj_ilog2_u64(1)==0, gj_ilog2_u64(2)==1,
 *       gj_ilog2_u64(3)==1, gj_ilog2_u64(UINT64_C(1)<<63)==63).
 *       Returns 0 when n == 0 (defined sentinel; log2(0) is undefined).
 *   uint64_t __gj_ilog2_u64  (alias)
 *   __libcgj_batch372_marker = "libcgj-batch372"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * floating point, no libc, no compiler builtins required. No third-party
 * source copied.
 */

#include <stdint.h>

const char __libcgj_batch372_marker[] = "libcgj-batch372";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ilog2_u64 — floor(log2(n)), or 0 if n == 0.
 *
 * Binary bit-scan (deconstruct high half first) so the loop is O(1)
 * with a fixed number of shift/compare steps rather than up to 63
 * single-bit shifts.
 */
uint64_t
gj_ilog2_u64(uint64_t u64N)
{
	uint64_t u64R;
	uint64_t u64X;

	if (u64N == 0u) {
		return 0u;
	}

	u64R = 0u;
	u64X = u64N;

	if (u64X >= 0x100000000ULL) {
		u64X >>= 32;
		u64R += 32u;
	}
	if (u64X >= 0x10000ULL) {
		u64X >>= 16;
		u64R += 16u;
	}
	if (u64X >= 0x100ULL) {
		u64X >>= 8;
		u64R += 8u;
	}
	if (u64X >= 0x10ULL) {
		u64X >>= 4;
		u64R += 4u;
	}
	if (u64X >= 0x4ULL) {
		u64X >>= 2;
		u64R += 2u;
	}
	if (u64X >= 0x2ULL) {
		u64R += 1u;
	}
	return u64R;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ilog2_u64(uint64_t u64N)
    __attribute__((alias("gj_ilog2_u64")));
