/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2694: paired dyn smoke soft-green predicate.
 *
 * Surface (unique symbols):
 *   int gj_dyn_green_pair_p(int code_a, int code_b);
 *     - Return 1 if both dyn-smoke codes are soft-green: each code is
 *       in [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for paired cgj_dyn_smoke results.
 *   int __gj_dyn_green_pair_p  (alias)
 *   __libcgj_batch2694_marker = "libcgj-batch2694"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_dyn_green_pair_p surface only; no multi-def. Distinct from
 * gj_smoke_green_pair_p (batch2693 exact 0 pair), gj_dyn_pass_p
 * (batch2496 single code band), gj_smoke_both_pass_p (batch2594),
 * and gj_dyn_smoke_ok_p (batch2396).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2694_marker[] = "libcgj-batch2694";

/* Inclusive upper bound of dyn-smoke soft-green band. */
#define B2694_DYN_GREEN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if one dyn code is in soft-green band [0, B2694_DYN_GREEN_MAX]. */
static int
b2694_one_green(int nCode)
{
	if (nCode < 0) {
		return 0;
	}
	if (nCode > B2694_DYN_GREEN_MAX) {
		return 0;
	}
	return 1;
}

/* 1 if both dyn codes are soft-green. */
static int
b2694_green_pair(int nCodeA, int nCodeB)
{
	if (b2694_one_green(nCodeA) == 0) {
		return 0;
	}
	if (b2694_one_green(nCodeB) == 0) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_pair_p - 1 iff both dyn-smoke codes are soft-green.
 *
 * code_a: first dyn-smoke result
 * code_b: second dyn-smoke result
 *
 * Soft-green band is [0, 2] for each code. Distinct from static green
 * pair (exact 0) and single-code dyn pass. Does not call libc.
 */
int
gj_dyn_green_pair_p(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2694_green_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dyn_green_pair_p(int nCodeA, int nCodeB)
    __attribute__((alias("gj_dyn_green_pair_p")));
