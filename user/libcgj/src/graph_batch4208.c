/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4208: freestanding floor log2 for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_ilog2_u(uint32_t x);
 *     - Floor of log2(x). x == 0 → 0 (defined special case).
 *   uint32_t __gj_u32_ilog2_u  (alias)
 *   __libcgj_batch4208_marker = "libcgj-batch4208"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210). Distinct from
 * gj_u32_ilog2 (batch776) / gj_u32_log2_floor (batch2241) — unique
 * gj_u32_ilog2_u continuum surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4208_marker[] = "libcgj-batch4208";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Binary search on bit width; no libm, no builtins required.
 * x must be non-zero (caller handles x == 0).
 */
static uint32_t
b4208_ilog2(uint32_t u32X)
{
	uint32_t u32R;

	u32R = 0u;
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
 * gj_u32_ilog2_u - floor(log2(x)); 0 maps to 0.
 *
 * x: input (0 → 0; 1 → 0; 2..3 → 1; ...; 0x80000000..0xffffffff → 31)
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_ilog2_u(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 0u;
	}
	return b4208_ilog2(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_ilog2_u(uint32_t u32X)
    __attribute__((alias("gj_u32_ilog2_u")));
