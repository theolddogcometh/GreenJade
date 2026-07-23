/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2594: dual smoke both-PASS predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_both_pass_p(int code_a, int code_b);
 *     - Return 1 if both smoke codes are strict PASS (exactly 0),
 *       else 0. Soft pure-data gate for paired static/dyn smoke lamps.
 *   int __gj_smoke_both_pass_p  (alias)
 *   __libcgj_batch2594_marker = "libcgj-batch2594"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_smoke_both_pass_p surface only; no multi-def. Distinct from
 * gj_smoke_pass_p (batch2494 single code), gj_smoke_green_p
 * (batch2394), and gj_dyn_pass_p (batch2496 band).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2594_marker[] = "libcgj-batch2594";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if both codes are strict PASS (exactly 0). */
static int
b2594_both_pass(int nCodeA, int nCodeB)
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
 * gj_smoke_both_pass_p - 1 iff both smoke codes are strict PASS.
 *
 * code_a: first smoke result (e.g. static cgj_smoke)
 * code_b: second smoke result (e.g. dyn cgj_dyn_smoke)
 *
 * PASS is exactly 0 for both. Does not call libc.
 */
int
gj_smoke_both_pass_p(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2594_both_pass(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_both_pass_p(int nCodeA, int nCodeB)
    __attribute__((alias("gj_smoke_both_pass_p")));
