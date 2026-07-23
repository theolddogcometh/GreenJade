/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch366: count trailing zero bits in a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_ctz64(uint64_t x);
 *     — Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 0 when x == 0 (defined convention for this surface;
 *       distinct from GCC __builtin_ctzll which is undefined at 0).
 *   unsigned __gj_ctz64  (alias)
 *   __libcgj_batch366_marker = "libcgj-batch366"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch366_marker[] = "libcgj-batch366";

/*
 * gj_ctz64 — count trailing zeros in a 64-bit word.
 *
 * x: value to inspect
 *
 * Binary-search de Bruijn-free path: peel 32/16/8/4/2/1 with masks.
 * Zero input is defined as 0 (caller convention for this API).
 */
unsigned
gj_ctz64(uint64_t u64X)
{
	unsigned uCtz;
	uint64_t u64V;

	if (u64X == 0ull) {
		return 0u;
	}

	u64V = u64X;
	uCtz = 0u;

	if ((u64V & 0xffffffffull) == 0ull) {
		uCtz += 32u;
		u64V >>= 32;
	}
	if ((u64V & 0xffffull) == 0ull) {
		uCtz += 16u;
		u64V >>= 16;
	}
	if ((u64V & 0xffull) == 0ull) {
		uCtz += 8u;
		u64V >>= 8;
	}
	if ((u64V & 0xfull) == 0ull) {
		uCtz += 4u;
		u64V >>= 4;
	}
	if ((u64V & 0x3ull) == 0ull) {
		uCtz += 2u;
		u64V >>= 2;
	}
	if ((u64V & 0x1ull) == 0ull) {
		uCtz += 1u;
	}

	return uCtz;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ctz64(uint64_t u64X)
    __attribute__((alias("gj_ctz64")));
