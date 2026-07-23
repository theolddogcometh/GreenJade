/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2693: paired static smoke green predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_green_pair_p(int code_a, int code_b);
 *     - Return 1 if both static smoke codes are strict green PASS
 *       (exactly 0), else 0. Soft pure-data gate for paired static
 *       smoke lamps (e.g. host + guest static smoke).
 *   int __gj_smoke_green_pair_p  (alias)
 *   __libcgj_batch2693_marker = "libcgj-batch2693"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_smoke_green_pair_p surface only; no multi-def. Distinct from
 * gj_smoke_both_pass_p (batch2594), gj_smoke_pass_p (batch2494),
 * gj_smoke_green_p (batch2394), and gj_dyn_green_pair_p (batch2694).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2693_marker[] = "libcgj-batch2693";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if both codes are strict green PASS (exactly 0). */
static int
b2693_green_pair(int nCodeA, int nCodeB)
{
	if (nCodeA != 0) {
		return 0;
	}
	if (nCodeB != 0) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_pair_p - 1 iff both static smoke codes are green.
 *
 * code_a: first static smoke result
 * code_b: second static smoke result
 *
 * Green PASS is exactly 0 for both. Pair with gj_dyn_green_pair_p for
 * the dyn soft-band counterpart. Does not call libc.
 */
int
gj_smoke_green_pair_p(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2693_green_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_green_pair_p(int nCodeA, int nCodeB)
    __attribute__((alias("gj_smoke_green_pair_p")));
