/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4117: uint64_t left shift with high-bit
 * overflow saturation to all-ones.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_sat_u2(uint64_t x, unsigned n);
 *     - Return x << n when the shift does not discard any high bits.
 *       If any high bit would be shifted out (overflow), saturate to
 *       all-ones (0xFFFFFFFFFFFFFFFF). For n >= 64: 0 if x == 0, else
 *       all-ones. Pure integer; no shift UB.
 *   uint64_t __gj_u64_shl_sat_u2  (alias)
 *   __libcgj_batch4117_marker = "libcgj-batch4117"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). High-bit saturation
 * semantics (all-1 on overflow), distinct from count-saturate-to-0
 * gj_u64_shl_sat (batch2405) and pair with gj_u32_shl_sat_u (batch4115).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4117_marker[] = "libcgj-batch4117";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Left shift with high-bit overflow → all-ones.
 * n >= 64: entire value shifts out → all1 if x != 0, else 0.
 * n in 1..63: if any of the top n bits of x is set, saturate.
 */
static uint64_t
b4117_shl_sat(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		if (u64X != 0ull) {
			return ~(uint64_t)0;
		}
		return 0ull;
	}
	if (uN == 0u) {
		return u64X;
	}
	if ((u64X >> (64u - uN)) != 0ull) {
		return ~(uint64_t)0;
	}
	return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_sat_u2 - left shift saturating high bits to all-ones on overflow.
 *
 * x: value to shift
 * n: shift count
 *
 * Returns x << n if no high bits are lost; otherwise all-ones.
 * No parent wires.
 */
uint64_t
gj_u64_shl_sat_u2(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b4117_shl_sat(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_sat_u2(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shl_sat_u2")));
