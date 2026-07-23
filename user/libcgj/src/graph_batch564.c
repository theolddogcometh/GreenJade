/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch564: count leading zero bits in a uint64_t.
 *
 * Surface (unique symbols; distinct from batch367 gj_clz64 / __gj_clz64):
 *   unsigned gj_clz_u64(uint64_t x);
 *     — Number of consecutive 0-bits starting from bit 63 (MSB).
 *       Returns 64 when x == 0 (fully defined for the zero input).
 *   unsigned __gj_clz_u64  (alias)
 *   __libcgj_batch564_marker = "libcgj-batch564"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch564_marker[] = "libcgj-batch564";

/*
 * gj_clz_u64 — count leading zeros in a 64-bit word.
 *
 * x: value to inspect
 *
 * Binary-search peel of 32/16/8/4/2/1 against high half-masks.
 * Zero input is defined as 64.
 */
unsigned
gj_clz_u64(uint64_t x)
{
	unsigned uClz;
	uint64_t u64V;

	if (x == 0ull) {
		return 64u;
	}

	u64V = x;
	uClz = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uClz += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uClz += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uClz += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uClz += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uClz += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uClz += 1u;
	}

	return uClz;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_clz_u64(uint64_t x)
    __attribute__((alias("gj_clz_u64")));
