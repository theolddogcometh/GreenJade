/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3105: constant-time u32 equality mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_eq_mask_u(uint32_t a, uint32_t b);
 *     - Returns all-bits-one (~0u) if a == b, else 0. Branch-free on
 *       a/b values (full-width mask suitable for AND/select chains).
 *   uint32_t __gj_u32_eq_mask_u  (alias)
 *   __libcgj_batch3105_marker = "libcgj-batch3105"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_ct_eq_mask_u64 (batch1513) — unique gj_u32_eq_mask_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3105_marker[] = "libcgj-batch3105";

/* ---- freestanding helpers ---------------------------------------------- */

/* a == b → ~0u; else 0. Branch-free. */
static uint32_t
b3105_eq_mask(uint32_t uA, uint32_t uB)
{
	uint32_t uX;
	uint32_t uNeg;
	uint32_t uOr;

	uX = uA ^ uB;
	/*
	 * uX == 0 → equal. Fold to a 0/1 bit without content branches:
	 * (x | -x) has MSB set iff x != 0; invert that bit for equal=1.
	 */
	uNeg = 0u - uX;
	uOr = uX | uNeg;
	return 0u - ((uOr >> 31) ^ 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_eq_mask_u - constant-time equality mask of two 32-bit words.
 *
 * a == b → 0xffffffffu; else 0. No parent wires.
 */
uint32_t
gj_u32_eq_mask_u(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3105_eq_mask(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_eq_mask_u(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_eq_mask_u")));
