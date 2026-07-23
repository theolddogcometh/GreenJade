/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2241: floor base-2 log for uint32_t
 * (post-2240 math exclusive wave 2241-2250).
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_log2_floor(uint32_t x);
 *     - Floor of log2(x). x == 0 -> 0 (defined sentinel).
 *       1 -> 0; 2..3 -> 1; ...; 0x80000000..0xffffffff -> 31.
 *   unsigned __gj_u32_log2_floor  (alias)
 *   __libcgj_batch2241_marker = "libcgj-batch2241"
 *
 * Distinct from gj_u32_ilog2 (batch776). Unique gj_u32_log2_floor
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2241_marker[] = "libcgj-batch2241";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary peel of highest set bit; x must be non-zero. */
static unsigned
b2241_log2_floor(uint32_t u32X)
{
	unsigned uR;

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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_log2_floor - floor(log2(x)); 0 maps to 0.
 *
 * x: input (0 -> 0; 1 -> 0; 2..3 -> 1; ...; 0x80000000..0xffffffff -> 31)
 *
 * Binary search on bit width; no libm, no builtins required.
 */
unsigned
gj_u32_log2_floor(uint32_t u32X)
{
	(void)NULL;
	if (u32X == 0u) {
		return 0u;
	}
	return b2241_log2_floor(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_log2_floor(uint32_t u32X)
    __attribute__((alias("gj_u32_log2_floor")));
