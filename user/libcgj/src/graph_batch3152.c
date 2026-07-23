/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3152: floor base-2 log for uint64_t (u-suffix).
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_log2_floor_u(uint64_t x);
 *     - Floor of log2(x). x == 0 -> 0 (defined sentinel).
 *       1 -> 0; 2..3 -> 1; ...; 1<<63 .. UINT64_MAX -> 63.
 *   unsigned __gj_u64_log2_floor_u  (alias)
 *   __libcgj_batch3152_marker = "libcgj-batch3152"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_u64_log2_floor (batch2242) / gj_ilog2_u64 (batch372) /
 * gj_floor_log2_u64 (batch579) — unique gj_u64_log2_floor_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3152_marker[] = "libcgj-batch3152";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary peel of highest set bit; x must be non-zero. */
static unsigned
b3152_log2_floor(uint64_t u64X)
{
	unsigned uR;

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
	return uR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_log2_floor_u - floor(log2(x)); 0 maps to 0.
 *
 * x: input (0 -> 0; 1 -> 0; 2..3 -> 1; ...; 1<<63 .. UINT64_MAX -> 63)
 *
 * Binary search on bit width; no libm, no builtins required.
 * No parent wires.
 */
unsigned
gj_u64_log2_floor_u(uint64_t u64X)
{
	(void)NULL;
	if (u64X == 0ull) {
		return 0u;
	}
	return b3152_log2_floor(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_log2_floor_u(uint64_t u64X)
    __attribute__((alias("gj_u64_log2_floor_u")));
