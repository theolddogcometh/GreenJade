/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8276: freestanding uint32 align-up.
 *
 * Surface (unique symbols):
 *   uint32_t gj_align_up_u32_8276(uint32_t v, uint32_t a);
 *     - Round v up to the next multiple of a.
 *       a == 0 or a == 1 → return v.
 *       If a is a power of two: mask form (v + a - 1) & ~(a - 1).
 *       Otherwise: general multiple form.
 *       On arithmetic overflow of the round-up, returns 0.
 *   uint32_t __gj_align_up_u32_8276  (alias)
 *   __libcgj_batch8276_marker = "libcgj-batch8276"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_align_up_u32_8276 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8276_marker[] = "libcgj-batch8276";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round v up to a multiple of a. a <= 1 → identity.
 * Overflow of the rounded value → 0.
 */
static uint32_t
b8276_align_up(uint32_t u32V, uint32_t u32A)
{
	uint32_t u32Mask;
	uint32_t u32Adj;
	uint32_t u32Rem;

	if (u32A <= 1u) {
		return u32V;
	}

	/* Power-of-two fast path. */
	if ((u32A & (u32A - 1u)) == 0u) {
		u32Mask = u32A - 1u;
		u32Adj = (u32V + u32Mask) & ~u32Mask;
		if (u32Adj < u32V) {
			return 0u;
		}
		return u32Adj;
	}

	/* General multiple: v + (a - v % a) when not already aligned. */
	u32Rem = u32V % u32A;
	if (u32Rem == 0u) {
		return u32V;
	}
	u32Adj = u32V + (u32A - u32Rem);
	if (u32Adj < u32V) {
		return 0u;
	}
	return u32Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_up_u32_8276 - round v up to a multiple of a.
 *
 * v: value to align
 * a: alignment / stride (0 or 1 → identity; pow2 uses mask path)
 *
 * Overflow (rounded value wraps below v) → 0.
 * No parent wires.
 */
uint32_t
gj_align_up_u32_8276(uint32_t u32V, uint32_t u32A)
{
	(void)NULL;
	return b8276_align_up(u32V, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_align_up_u32_8276(uint32_t u32V, uint32_t u32A)
    __attribute__((alias("gj_align_up_u32_8276")));
