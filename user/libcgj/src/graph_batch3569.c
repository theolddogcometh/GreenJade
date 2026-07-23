/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3569: contiguous inclusive bit mask over
 * [from, to] in a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mask_from_to_u(unsigned from, unsigned to);
 *     - Return a mask with bits from inclusive through to inclusive
 *       set. from > to → 0. Bits at index >= 64 are ignored (to
 *       clamped at 63; from >= 64 → 0). Example: from=2, to=4 →
 *       bits 2,3,4 → 0x1C.
 *   uint64_t __gj_u64_mask_from_to_u  (alias)
 *   __libcgj_batch3569_marker = "libcgj-batch3569"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570). Companion to
 * gj_u32_mask_from_to_u (batch3568). Distinct from
 * gj_bit_mask_between (batch1011 half-open) — unique inclusive u64
 * from_to surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3569_marker[] = "libcgj-batch3569";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inclusive mask bits [from, to]. from/to already validated:
 * from <= to < 64. Width = to - from + 1 in 1..64.
 */
static uint64_t
b3569_mask(unsigned uFrom, unsigned uTo)
{
	unsigned uWidth;
	uint64_t uLow;

	uWidth = uTo - uFrom + 1u;
	if (uWidth >= 64u) {
		/* Only reachable when from == 0 and to == 63. */
		return ~(uint64_t)0;
	}

	uLow = ((uint64_t)1 << uWidth) - (uint64_t)1;
	return uLow << uFrom;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mask_from_to_u - bits [from, to] inclusive set in a uint64_t.
 *
 * from: first bit index (0 = LSB), inclusive
 * to:   last bit index, inclusive
 *
 * Safe against shift-by-width UB. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_mask_from_to_u(unsigned uFrom, unsigned uTo)
{
	(void)NULL;
	if (uFrom > uTo || uFrom >= 64u) {
		return 0ull;
	}
	if (uTo >= 64u) {
		uTo = 63u;
	}
	return b3569_mask(uFrom, uTo);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mask_from_to_u(unsigned uFrom, unsigned uTo)
    __attribute__((alias("gj_u64_mask_from_to_u")));
