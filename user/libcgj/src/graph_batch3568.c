/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3568: contiguous inclusive bit mask over
 * [from, to] in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mask_from_to_u(unsigned from, unsigned to);
 *     - Return a mask with bits from inclusive through to inclusive
 *       set. from > to → 0. Bits at index >= 32 are ignored (to
 *       clamped at 31; from >= 32 → 0). Example: from=2, to=4 →
 *       bits 2,3,4 → 0x1C.
 *   uint32_t __gj_u32_mask_from_to_u  (alias)
 *   __libcgj_batch3568_marker = "libcgj-batch3568"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Distinct from
 * gj_bit_mask_between (batch1011 half-open u64) — unique inclusive
 * u32 from_to surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3568_marker[] = "libcgj-batch3568";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inclusive mask bits [from, to]. from/to already validated:
 * from <= to < 32. Width = to - from + 1 in 1..32.
 */
static uint32_t
b3568_mask(unsigned uFrom, unsigned uTo)
{
	unsigned uWidth;
	uint32_t uLow;

	uWidth = uTo - uFrom + 1u;
	if (uWidth >= 32u) {
		/* Only reachable when from == 0 and to == 31. */
		return ~(uint32_t)0u;
	}

	uLow = ((uint32_t)1u << uWidth) - 1u;
	return uLow << uFrom;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mask_from_to_u - bits [from, to] inclusive set in a uint32_t.
 *
 * from: first bit index (0 = LSB), inclusive
 * to:   last bit index, inclusive
 *
 * Safe against shift-by-width UB. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_mask_from_to_u(unsigned uFrom, unsigned uTo)
{
	(void)NULL;
	if (uFrom > uTo || uFrom >= 32u) {
		return 0u;
	}
	if (uTo >= 32u) {
		uTo = 31u;
	}
	return b3568_mask(uFrom, uTo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mask_from_to_u(unsigned uFrom, unsigned uTo)
    __attribute__((alias("gj_u32_mask_from_to_u")));
