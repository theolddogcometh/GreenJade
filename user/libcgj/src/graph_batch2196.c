/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2196: thousand-aligned milestone predicate.
 *
 * Surface (unique symbols):
 *   int gj_milestone_is_thousand_p(uint32_t m);
 *     - Return 1 if m is a positive multiple of 1000 (1000, 2000,
 *       3000, ...), else 0. Zero is not a milestone thousand.
 *   int __gj_milestone_is_thousand_p  (alias)
 *   __libcgj_batch2196_marker = "libcgj-batch2196"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Distinct from
 * gj_milestone_is_hundred_p (batch2195); unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2196_marker[] = "libcgj-batch2196";

/* Thousand stride for product milestone grid. */
#define B2196_THOUSAND  1000u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if m is nonzero and m % 1000 == 0. */
static int
b2196_thousand_p(uint32_t uM)
{
	if (uM == 0u) {
		return 0;
	}
	return (uM % B2196_THOUSAND) == 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_is_thousand_p - true if m is a positive 1000-aligned id.
 *
 * m: candidate graph milestone number
 *
 * Returns 1 when m > 0 and m is divisible by 1000, else 0.
 * Examples: 2000 -> 1, 1000 -> 1, 2200 -> 0, 0 -> 0.
 * Does not call libc.
 */
int
gj_milestone_is_thousand_p(uint32_t uM)
{
	(void)NULL;
	return b2196_thousand_p(uM);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_milestone_is_thousand_p(uint32_t uM)
    __attribute__((alias("gj_milestone_is_thousand_p")));
