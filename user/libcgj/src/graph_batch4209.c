/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4209: freestanding floor log2 for uint64_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_ilog2_u(uint64_t x);
 *     - Floor of log2(x). x == 0 → 0 (defined special case).
 *       Result fits in uint32_t (0..63).
 *   uint32_t __gj_u64_ilog2_u  (alias)
 *   __libcgj_batch4209_marker = "libcgj-batch4209"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u32_ilog2_u (batch4208) / gj_u32_ilog2 (batch776) — unique
 * gj_u64_ilog2_u continuum surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4209_marker[] = "libcgj-batch4209";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary search on bit width; no libm, no builtins required.
 * x must be non-zero (caller handles x == 0).
 */
static uint32_t
b4209_ilog2(uint64_t u64X)
{
	uint32_t u32R;

	u32R = 0u;
	if (u64X >= 0x100000000ULL) {
		u64X >>= 32;
		u32R += 32u;
	}
	if (u64X >= 0x10000ULL) {
		u64X >>= 16;
		u32R += 16u;
	}
	if (u64X >= 0x100ULL) {
		u64X >>= 8;
		u32R += 8u;
	}
	if (u64X >= 0x10ULL) {
		u64X >>= 4;
		u32R += 4u;
	}
	if (u64X >= 0x4ULL) {
		u64X >>= 2;
		u32R += 2u;
	}
	if (u64X >= 0x2ULL) {
		u32R += 1u;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ilog2_u - floor(log2(x)); 0 maps to 0.
 *
 * x: input (0 → 0; 1 → 0; 2..3 → 1; ...; 1<<63 .. UINT64_MAX → 63)
 * Self-contained; no parent wires.
 */
uint32_t
gj_u64_ilog2_u(uint64_t u64X)
{
	(void)NULL;
	if (u64X == 0ULL) {
		return 0u;
	}
	return b4209_ilog2(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_ilog2_u(uint64_t u64X)
    __attribute__((alias("gj_u64_ilog2_u")));
