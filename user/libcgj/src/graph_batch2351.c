/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2351: median of three uint32_t values
 * (post-2350 exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_median3(uint32_t a, uint32_t b, uint32_t c);
 *     - Return the median (middle value) of a, b, and c under
 *       unsigned 32-bit order. Ties may return any equal middle.
 *   uint32_t __gj_u32_median3  (alias)
 *   __libcgj_batch2351_marker = "libcgj-batch2351"
 *
 * Distinct from gj_median3_u64 (batch590) and gj_u32_median_sorted
 * (batch908) - unique gj_u32_median3 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2351_marker[] = "libcgj-batch2351";

/* ---- freestanding helpers ---------------------------------------------- */

/* Median of three uint32 values via between-tests. */
static uint32_t
b2351_median3(uint32_t uA, uint32_t uB, uint32_t uC)
{
	if ((uB <= uA && uA <= uC) || (uC <= uA && uA <= uB)) {
		return uA;
	}
	if ((uA <= uB && uB <= uC) || (uC <= uB && uB <= uA)) {
		return uB;
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_median3 - median of three uint32_t values.
 *
 * a, b, c: candidates
 *
 * Returns the middle value when the three are ordered under unsigned
 * comparison. Does not call libc.
 */
uint32_t
gj_u32_median3(uint32_t uA, uint32_t uB, uint32_t uC)
{
	(void)NULL;
	return b2351_median3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_median3(uint32_t uA, uint32_t uB, uint32_t uC)
    __attribute__((alias("gj_u32_median3")));
