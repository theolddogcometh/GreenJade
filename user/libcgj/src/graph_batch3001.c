/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3001: ceiling base-2 log for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_log2_ceil_u(uint32_t n);
 *     - Smallest k such that 2^k >= n for n >= 1.
 *       n == 0 -> 0 (defined sentinel).
 *       n == 1 -> 0; n == 2 -> 1; n == 3 -> 2; n == 4 -> 2.
 *   unsigned __gj_u32_log2_ceil_u  (alias)
 *   __libcgj_batch3001_marker = "libcgj-batch3001"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010). Distinct from
 * gj_u32_log2_ceil (batch2243) — unique gj_u32_log2_ceil_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3001_marker[] = "libcgj-batch3001";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(log2(n)) for n > 1 via floor(log2(n - 1)) + 1 binary peel of
 * the highest set bit of (n - 1).
 */
static unsigned
b3001_log2_ceil(uint32_t u32N)
{
	uint32_t u32X;
	unsigned uR;

	u32X = u32N - 1u;
	uR = 0u;

	if (u32X >= 0x10000u) {
		u32X >>= 16;
		uR += 16u;
	}
	if (u32X >= 0x100u) {
		u32X >>= 8;
		uR += 8u;
	}
	if (u32X >= 0x10u) {
		u32X >>= 4;
		uR += 4u;
	}
	if (u32X >= 0x4u) {
		u32X >>= 2;
		uR += 2u;
	}
	if (u32X >= 0x2u) {
		uR += 1u;
	}
	return uR + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_log2_ceil_u - ceil(log2(n)) for n >= 1; 0 when n == 0.
 *
 * Equivalent to floor(log2(n - 1)) + 1 for n > 1. No parent wires.
 */
unsigned
gj_u32_log2_ceil_u(uint32_t u32N)
{
	(void)NULL;
	if (u32N <= 1u) {
		return 0u;
	}
	return b3001_log2_ceil(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_log2_ceil_u(uint32_t u32N)
    __attribute__((alias("gj_u32_log2_ceil_u")));
