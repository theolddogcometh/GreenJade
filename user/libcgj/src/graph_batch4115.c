/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4115: uint32_t left shift with high-bit
 * overflow saturation to all-ones.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shl_sat_u2(uint32_t x, unsigned n);
 *     - Return x << n when the shift does not discard any high bits.
 *       If any high bit would be shifted out (overflow), saturate to
 *       all-ones (0xFFFFFFFF). For n >= 32: 0 if x == 0, else all-ones
 *       (all bits leave the word). Pure integer; no shift UB.
 *   uint32_t __gj_u32_shl_sat_u2  (alias)
 *   __libcgj_batch4115_marker = "libcgj-batch4115"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). High-bit saturation
 * semantics (all-1 on overflow), distinct from count-saturate-to-0
 * gj_u32_shl_sat (batch2407). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4115_marker[] = "libcgj-batch4115";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Left shift with high-bit overflow → all-ones.
 * n >= 32: entire value shifts out → all1 if x != 0, else 0.
 * n in 1..31: if any of the top n bits of x is set, saturate.
 */
static uint32_t
b4115_shl_sat(uint32_t u32X, unsigned uN)
{
	if (uN >= 32u) {
		if (u32X != 0u) {
			return 0xFFFFFFFFu;
		}
		return 0u;
	}
	if (uN == 0u) {
		return u32X;
	}
	if ((u32X >> (32u - uN)) != 0u) {
		return 0xFFFFFFFFu;
	}
	return u32X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shl_sat_u2 - left shift saturating high bits to all-ones on overflow.
 *
 * x: value to shift
 * n: shift count
 *
 * Returns x << n if no high bits are lost; otherwise 0xFFFFFFFF.
 * No parent wires.
 */
uint32_t
gj_u32_shl_sat_u2(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4115_shl_sat(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shl_sat_u2(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shl_sat_u2")));
