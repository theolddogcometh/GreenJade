/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2899: paired dyn-smoke soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_dyn_pair_u(int code_a, int code_b);
 *     - Return 1 if both dyn-smoke codes are soft-green: each code is
 *       in [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for paired cgj_dyn_smoke results.
 *       Unsigned 0/1 form.
 *   uint32_t __gj_product_dyn_pair_u  (alias)
 *   __libcgj_batch2899_marker = "libcgj-batch2899"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_product_dyn_pair_u surface only; no multi-def. Distinct from
 * gj_dyn_green_pair_p (batch2694 int), gj_product_smoke_pair_u
 * (batch2898 exact 0 pair), and gj_dyn_pass_p (batch2496).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2899_marker[] = "libcgj-batch2899";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B2899_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if one dyn code is in soft-green band [0, B2899_DYN_GREEN_MAX]. */
static uint32_t
b2899_one_green(int nCode)
{
	if (nCode < 0) {
		return 0u;
	}
	if (nCode > B2899_DYN_GREEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* 1 if both dyn codes are soft-green. */
static uint32_t
b2899_dyn_pair(int nCodeA, int nCodeB)
{
	if (b2899_one_green(nCodeA) == 0u) {
		return 0u;
	}
	if (b2899_one_green(nCodeB) == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_dyn_pair_u - 1 iff both dyn-smoke codes are soft-green.
 *
 * code_a: first dyn-smoke result
 * code_b: second dyn-smoke result
 *
 * Soft-green band is [0, 2] for each code. Distinct from static green
 * pair (exact 0) and single-code dyn pass. Does not call libc.
 */
uint32_t
gj_product_dyn_pair_u(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2899_dyn_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_dyn_pair_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_product_dyn_pair_u")));
