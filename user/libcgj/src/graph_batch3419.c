/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3419: soft PDEP parallel bit deposit (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pdep_soft_u(uint64_t src, uint64_t mask);
 *     - BMI2 PDEP software: deposit contiguous low bits of src into
 *       the bit positions indicated by mask (LSB of src fills the
 *       lowest set mask bit, next src bit the next set mask bit, …).
 *       Bits of the result outside mask are zero.
 *       gj_u64_pdep_soft_u(0x3, 0x101) == 0x101,
 *       gj_u64_pdep_soft_u(0xff, 0x0f0f) == 0x0f0f,
 *       gj_u64_pdep_soft_u(x, 0) == 0.
 *   uint64_t __gj_u64_pdep_soft_u  (alias)
 *   __libcgj_batch3419_marker = "libcgj-batch3419"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u64_pdep_soft_u
 * surface only; no multi-def. Distinct from gj_u32_pdep_soft_u
 * (batch3418) and gj_u64_deposit_bits_u (batch3233). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128. No BMI2 builtins.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3419_marker[] = "libcgj-batch3419";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PDEP: for each set bit of mask (low to high), take the next
 * low bit of src and place it at that mask position.
 */
static uint64_t
b3419_pdep(uint64_t u64Src, uint64_t u64Mask)
{
	uint64_t u64Result;
	uint64_t u64Bb;
	unsigned uI;

	u64Result = UINT64_C(0);
	u64Bb = UINT64_C(1);

	for (uI = 0u; uI < 64u; uI++) {
		if ((u64Mask & u64Bb) != UINT64_C(0)) {
			if ((u64Src & UINT64_C(1)) != UINT64_C(0)) {
				u64Result |= u64Bb;
			}
			u64Src >>= 1;
		}
		/* After bit 63, u64Bb <<= 1 becomes 0; loop ends next. */
		u64Bb <<= 1;
	}

	return u64Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pdep_soft_u - software parallel bit deposit (64-bit).
 *
 * src:  contiguous source bits (consumed low-to-high)
 * mask: destination bit positions (1 = place next src bit here)
 *
 * Pure integer; no BMI2. No parent wires.
 */
uint64_t
gj_u64_pdep_soft_u(uint64_t u64Src, uint64_t u64Mask)
{
	(void)NULL;
	return b3419_pdep(u64Src, u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pdep_soft_u(uint64_t u64Src, uint64_t u64Mask)
    __attribute__((alias("gj_u64_pdep_soft_u")));
