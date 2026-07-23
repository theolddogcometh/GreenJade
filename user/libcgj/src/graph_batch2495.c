/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2495: product smoke FAIL predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_fail_p(int code);
 *     - Return 1 if code is FAIL (any non-zero), else 0.
 *       Soft pure-data gate complementary to gj_smoke_pass_p.
 *   int __gj_smoke_fail_p  (alias)
 *   __libcgj_batch2495_marker = "libcgj-batch2495"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_smoke_fail_p surface only; no multi-def. Distinct from
 * gj_smoke_pass_p (batch2494), gj_smoke_red_p (batch2395), and
 * gj_smoke_gate_hard_p (batch1996).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2495_marker[] = "libcgj-batch2495";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is non-zero (FAIL lamp). */
static int
b2495_fail_p(int nCode)
{
	if (nCode != 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_fail_p - 1 iff smoke code is FAIL (non-zero).
 *
 * code: integer smoke result code (signed so negatives are fail).
 * FAIL is any code != 0. Pair with gj_smoke_pass_p for the complementary
 * pass lamp. Does not call libc.
 */
int
gj_smoke_fail_p(int nCode)
{
	(void)NULL;
	return b2495_fail_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_fail_p(int nCode)
    __attribute__((alias("gj_smoke_fail_p")));
