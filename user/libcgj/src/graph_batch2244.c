/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2244: ceiling base-2 log for uint64_t
 * (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_log2_ceil_soft(uint64_t n);
 *     - Smallest k such that 2^k >= n for n >= 1.
 *       n == 0 -> 0 (defined sentinel).
 *       n == 1 -> 0; n == 2 -> 1; n == 3 -> 2; n == 4 -> 2.
 *   unsigned __gj_u64_log2_ceil_soft  (alias)
 *   __libcgj_batch2244_marker = "libcgj-batch2244"
 *
 * Renamed from the planned gj_u64_log2_ceil because that symbol is
 * owned by batch1066; _soft suffix keeps the surface unique (no
 * multi-def). Also distinct from gj_ilog2_u64 (batch372) /
 * gj_floor_log2_u64 (batch579).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2244_marker[] = "libcgj-batch2244";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(log2(n)) for n > 1 via floor(log2(n - 1)) + 1 binary peel of
 * the highest set bit of (n - 1).
 */
static unsigned
b2244_log2_ceil(uint64_t u64N)
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
 * gj_u64_log2_ceil_soft - ceil(log2(n)) for n >= 1; 0 when n == 0.
 *
 * Equivalent to floor(log2(n - 1)) + 1 for n > 1.
 */
unsigned
gj_u64_log2_ceil_soft(uint64_t u64N)
{
	(void)NULL;
	if (u64N <= 1ull) {
		return 0u;
	}
	return b2244_log2_ceil(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_log2_ceil_soft(uint64_t u64N)
    __attribute__((alias("gj_u64_log2_ceil_soft")));
