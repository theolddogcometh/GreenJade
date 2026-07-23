/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8322: qsort/bsearch u32 equality cmp.
 *
 * Surface (unique symbols):
 *   int gj_cmp_u32_eq_8322(uint32_t a, uint32_t b);
 *     - Equality-oriented compare: 0 if a == b (bsearch match), else 1.
 *   int __gj_cmp_u32_eq_8322  (alias)
 *   __libcgj_batch8322_marker = "libcgj-batch8322"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Unique gj_cmp_u32_eq_8322 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8322_marker[] = "libcgj-batch8322";

/* ---- freestanding helpers ---------------------------------------------- */

/* 0 iff equal; 1 otherwise (bsearch-style match sentinel). */
static int
b8322_cmp_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmp_u32_eq_8322 - equality compare for u32 (0 = match).
 *
 * a, b: values under comparison
 *
 * Returns 0 when a == b, else 1. Distinct from three-way lt/gt helpers.
 * No parent wires.
 */
int
gj_cmp_u32_eq_8322(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8322_cmp_eq(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cmp_u32_eq_8322(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_cmp_u32_eq_8322")));
