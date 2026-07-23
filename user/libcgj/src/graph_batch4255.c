/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4255: soft parallel bit deposit low (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_deposit_low_u(uint32_t x, uint32_t mask);
 *     - Software PDEP-like: deposit contiguous low bits of x into the
 *       bit positions indicated by mask (LSB of x fills the lowest set
 *       mask bit, next x bit the next set mask bit, …). Bits of the
 *       result outside mask are zero.
 *       gj_u32_bit_deposit_low_u(0x3, 0x101) == 0x101,
 *       gj_u32_bit_deposit_low_u(0xff, 0x0f0f) == 0x0f0f,
 *       gj_u32_bit_deposit_low_u(x, 0) == 0.
 *   uint32_t __gj_u32_bit_deposit_low_u  (alias)
 *   __libcgj_batch4255_marker = "libcgj-batch4255"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique
 * gj_u32_bit_deposit_low_u surface only; no multi-def. Distinct from
 * gj_u32_pdep_soft_u (batch3418) and gj_bit_deposit_contiguous
 * (batch1012). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No BMI2 builtins.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4255_marker[] = "libcgj-batch4255";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PDEP loop: for each set bit of mask (low to high), take the next
 * low bit of x and place it at that mask position.
 */
static uint32_t
b4255_deposit(uint32_t u32X, uint32_t u32Mask)
{
	uint32_t u32Result;
	uint32_t u32Bb;
	unsigned uI;

	u32Result = 0u;
	u32Bb = 1u;

	for (uI = 0u; uI < 32u; uI++) {
		if ((u32Mask & u32Bb) != 0u) {
			if ((u32X & 1u) != 0u) {
				u32Result |= u32Bb;
			}
			u32X >>= 1;
		}
		/* After bit 31, u32Bb <<= 1 becomes 0; loop ends next. */
		u32Bb <<= 1;
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_deposit_low_u - software parallel bit deposit (32-bit).
 *
 * x:    contiguous source bits (consumed low-to-high)
 * mask: destination bit positions (1 = place next x bit here)
 *
 * Pure integer soft PDEP loop; no BMI2. No parent wires.
 */
uint32_t
gj_u32_bit_deposit_low_u(uint32_t u32X, uint32_t u32Mask)
{
	(void)NULL;
	return b4255_deposit(u32X, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_deposit_low_u(uint32_t u32X, uint32_t u32Mask)
    __attribute__((alias("gj_u32_bit_deposit_low_u")));
