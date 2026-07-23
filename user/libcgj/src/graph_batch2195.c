/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2195: hundred-aligned milestone predicate.
 *
 * Surface (unique symbols):
 *   int gj_milestone_is_hundred_p(uint32_t m);
 *     - Return 1 if m is a positive multiple of 100 (100, 200, ...,
 *       2200, ...), else 0. Zero is not a milestone hundred.
 *   int __gj_milestone_is_hundred_p  (alias)
 *   __libcgj_batch2195_marker = "libcgj-batch2195"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_milestone_is_hundred_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2195_marker[] = "libcgj-batch2195";

/* Hundred stride for product milestone grid. */
#define B2195_HUNDRED  100u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if m is nonzero and m % 100 == 0. */
static int
b2195_hundred_p(uint32_t uM)
{
	if (uM == 0u) {
		return 0;
	}
	return (uM % B2195_HUNDRED) == 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_is_hundred_p - true if m is a positive 100-aligned id.
 *
 * m: candidate graph milestone number
 *
 * Returns 1 when m > 0 and m is divisible by 100, else 0.
 * Examples: 2200 -> 1, 2100 -> 1, 2191 -> 0, 0 -> 0.
 * Does not call libc.
 */
int
gj_milestone_is_hundred_p(uint32_t uM)
{
	(void)NULL;
	return b2195_hundred_p(uM);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_milestone_is_hundred_p(uint32_t uM)
    __attribute__((alias("gj_milestone_is_hundred_p")));
