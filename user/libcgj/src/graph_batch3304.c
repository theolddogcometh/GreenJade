/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3304: floor base-2 logarithm for uint64_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_flog2_u(uint64_t x);
 *     - Floor of log2(x) for x >= 1 (MSB position, 0-based):
 *       gj_u64_flog2_u(1)==0, gj_u64_flog2_u(2)==1,
 *       gj_u64_flog2_u(3)==1, gj_u64_flog2_u(1ULL<<63)==63.
 *       Returns 0 when x == 0 (defined sentinel; log2(0) undefined).
 *   uint32_t __gj_u64_flog2_u  (alias)
 *   __libcgj_batch3304_marker = "libcgj-batch3304"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_ilog2_u64 (batch372 returns uint64_t) — unique flog2_u surface
 * returning uint32_t. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3304_marker[] = "libcgj-batch3304";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor log2 for x >= 1; caller must not pass 0. */
static uint32_t
b3304_flog2(uint64_t u64X)
{
	uint32_t uR;

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
 * gj_u64_flog2_u - floor base-2 logarithm of x.
 *
 * x: unsigned 64-bit value
 *
 * Returns floor(log2(x)) for x >= 1 as a uint32_t (range 0..63);
 * 0 when x == 0 (defined sentinel). No parent wires.
 */
uint32_t
gj_u64_flog2_u(uint64_t x)
{
	(void)NULL;
	if (x == 0ULL) {
		return 0u;
	}
	return b3304_flog2(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_flog2_u(uint64_t x)
    __attribute__((alias("gj_u64_flog2_u")));
