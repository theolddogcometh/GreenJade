/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3106: constant-time u64 equality mask (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_eq_mask_u(uint64_t a, uint64_t b);
 *     - Returns all-bits-one (~0ull) if a == b, else 0. Branch-free on
 *       a/b values (full-width mask suitable for AND/select chains).
 *   uint64_t __gj_u64_eq_mask_u  (alias)
 *   __libcgj_batch3106_marker = "libcgj-batch3106"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_ct_eq_mask_u64 (batch1513) / gj_ct_eq_u64 (batch1253) — unique
 * gj_u64_eq_mask_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3106_marker[] = "libcgj-batch3106";

/* ---- freestanding helpers ---------------------------------------------- */

/* a == b → ~0ull; else 0. Branch-free. */
static uint64_t
b3106_eq_mask(uint64_t uA, uint64_t uB)
{
	uint64_t uX;
	uint64_t uNeg;
	uint64_t uOr;

	uX = uA ^ uB;
	/*
	 * uX == 0 → equal. Fold to a 0/1 bit without content branches:
	 * (x | -x) has MSB set iff x != 0; invert that bit for equal=1.
	 */
	uNeg = 0ull - uX;
	uOr = uX | uNeg;
	return 0ull - ((uOr >> 63) ^ 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_eq_mask_u - constant-time equality mask of two 64-bit words.
 *
 * a == b → 0xffffffffffffffffull; else 0. No parent wires.
 */
uint64_t
gj_u64_eq_mask_u(uint64_t uA, uint64_t uB)
{
	(void)NULL;
	return b3106_eq_mask(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_eq_mask_u(uint64_t uA, uint64_t uB)
    __attribute__((alias("gj_u64_eq_mask_u")));
