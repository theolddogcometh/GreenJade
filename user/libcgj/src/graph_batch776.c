/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch776: freestanding floor log2 for uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_ilog2(uint32_t x);
 *     — Floor of log2(x). x == 0 → 0 (defined special case).
 *   unsigned __gj_u32_ilog2  (alias)
 *   __libcgj_batch776_marker = "libcgj-batch776"
 *
 * Unique gj_u32_ilog2 surface only; no multi-def with sibling pow2
 * helpers (batches 777–779).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch776_marker[] = "libcgj-batch776";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_ilog2 — floor(log2(x)); 0 maps to 0.
 *
 * x: input (0 → 0; 1 → 0; 2..3 → 1; ...; 0x80000000..0xffffffff → 31)
 *
 * Binary search on bit width; no libm, no builtins required.
 */
unsigned
gj_u32_ilog2(uint32_t u32X)
{
	unsigned uR;

	if (u32X == 0u) {
		return 0u;
	}
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
	return uR;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_ilog2(uint32_t u32X)
    __attribute__((alias("gj_u32_ilog2")));
