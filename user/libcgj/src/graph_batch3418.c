/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3418: soft PDEP parallel bit deposit (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pdep_soft_u(uint32_t src, uint32_t mask);
 *     - BMI2 PDEP software: deposit contiguous low bits of src into
 *       the bit positions indicated by mask (LSB of src fills the
 *       lowest set mask bit, next src bit the next set mask bit, …).
 *       Bits of the result outside mask are zero.
 *       gj_u32_pdep_soft_u(0x3, 0x101) == 0x101,
 *       gj_u32_pdep_soft_u(0xff, 0x0f0f) == 0x0f0f,
 *       gj_u32_pdep_soft_u(x, 0) == 0.
 *   uint32_t __gj_u32_pdep_soft_u  (alias)
 *   __libcgj_batch3418_marker = "libcgj-batch3418"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u32_pdep_soft_u
 * surface only; no multi-def. Distinct from gj_u32_deposit_bits_u
 * (batch3232) which inserts a contiguous field. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No BMI2 builtins.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3418_marker[] = "libcgj-batch3418";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PDEP: for each set bit of mask (low to high), take the next
 * low bit of src and place it at that mask position.
 */
static uint32_t
b3418_pdep(uint32_t u32Src, uint32_t u32Mask)
{
	uint32_t u32Result;
	uint32_t u32Bb;
	unsigned uI;

	u32Result = 0u;
	u32Bb = 1u;

	for (uI = 0u; uI < 32u; uI++) {
		if ((u32Mask & u32Bb) != 0u) {
			if ((u32Src & 1u) != 0u) {
				u32Result |= u32Bb;
			}
			u32Src >>= 1;
		}
		/* After bit 31, u32Bb <<= 1 becomes 0; loop ends next. */
		u32Bb <<= 1;
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pdep_soft_u - software parallel bit deposit (32-bit).
 *
 * src:  contiguous source bits (consumed low-to-high)
 * mask: destination bit positions (1 = place next src bit here)
 *
 * Pure integer; no BMI2. No parent wires.
 */
uint32_t
gj_u32_pdep_soft_u(uint32_t u32Src, uint32_t u32Mask)
{
	(void)NULL;
	return b3418_pdep(u32Src, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pdep_soft_u(uint32_t u32Src, uint32_t u32Mask)
    __attribute__((alias("gj_u32_pdep_soft_u")));
