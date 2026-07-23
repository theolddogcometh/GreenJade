/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2783: floor base-2 logarithm for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ilog2_u32_u(uint32_t n);
 *     - Floor of log2(n) for n >= 1 (position of the highest set bit,
 *       0-based: gj_ilog2_u32_u(1)==0, gj_ilog2_u32_u(2)==1,
 *       gj_ilog2_u32_u(3)==1, gj_ilog2_u32_u(1u<<31)==31).
 *       Returns 0 when n == 0 (defined sentinel; log2(0) is undefined).
 *   uint32_t __gj_ilog2_u32_u  (alias)
 *   __libcgj_batch2783_marker = "libcgj-batch2783"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from gj_ilog2_u64
 * (batch372) — unique _u u32 surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied. No compiler builtins
 * required.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2783_marker[] = "libcgj-batch2783";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor log2 of u32N, or 0 if u32N == 0.
 * Binary bit-scan (high half first) so the loop is O(1) with a fixed
 * number of shift/compare steps rather than up to 31 single-bit shifts.
 */
static uint32_t
b2783_ilog2(uint32_t u32N)
{
	uint32_t u32R;
	uint32_t u32X;

	if (u32N == 0u) {
		return 0u;
	}

	u32R = 0u;
	u32X = u32N;

	if (u32X >= 0x10000u) {
		u32X >>= 16;
		u32R += 16u;
	}
	if (u32X >= 0x100u) {
		u32X >>= 8;
		u32R += 8u;
	}
	if (u32X >= 0x10u) {
		u32X >>= 4;
		u32R += 4u;
	}
	if (u32X >= 0x4u) {
		u32X >>= 2;
		u32R += 2u;
	}
	if (u32X >= 0x2u) {
		u32R += 1u;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ilog2_u32_u - floor(log2(n)), or 0 if n == 0.
 *
 * n: input value
 *
 * Returns the 0-based index of the highest set bit for n >= 1, else 0.
 */
uint32_t
gj_ilog2_u32_u(uint32_t u32N)
{
	(void)NULL;
	return b2783_ilog2(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ilog2_u32_u(uint32_t u32N)
    __attribute__((alias("gj_ilog2_u32_u")));
