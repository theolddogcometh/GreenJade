/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2784: floor base-2 logarithm for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ilog2_u64_u(uint64_t n);
 *     - Floor of log2(n) for n >= 1 (position of the highest set bit,
 *       0-based: gj_ilog2_u64_u(1)==0, gj_ilog2_u64_u(2)==1,
 *       gj_ilog2_u64_u(3)==1, gj_ilog2_u64_u(UINT64_C(1)<<63)==63).
 *       Returns 0 when n == 0 (defined sentinel; log2(0) is undefined).
 *   uint64_t __gj_ilog2_u64_u  (alias)
 *   __libcgj_batch2784_marker = "libcgj-batch2784"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from gj_ilog2_u64
 * (batch372) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied. No compiler builtins
 * required.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2784_marker[] = "libcgj-batch2784";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor log2 of u64N, or 0 if u64N == 0.
 * Binary bit-scan (high half first) so the loop is O(1) with a fixed
 * number of shift/compare steps rather than up to 63 single-bit shifts.
 */
static uint64_t
b2784_ilog2(uint64_t u64N)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ilog2_u64_u - floor(log2(n)), or 0 if n == 0.
 *
 * n: input value
 *
 * Returns the 0-based index of the highest set bit for n >= 1, else 0.
 */
uint64_t
gj_ilog2_u64_u(uint64_t u64N)
{
	(void)NULL;
	return b2784_ilog2(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ilog2_u64_u(uint64_t u64N)
    __attribute__((alias("gj_ilog2_u64_u")));
