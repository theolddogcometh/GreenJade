/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2199: hard-fail smoke result predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_hard_fail_p(int code);
 *     - Return 1 if code is a hard failure: negative, or outside the
 *       soft-acceptable band (code > 3). Else 0. Distinct from
 *       gj_smoke_gate_hard_p (batch1996 strict PASS-only gate).
 *   int __gj_smoke_hard_fail_p  (alias)
 *   __libcgj_batch2199_marker = "libcgj-batch2199"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_smoke_hard_fail_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2199_marker[] = "libcgj-batch2199";

/* Soft-acceptable band upper bound; above this is hard fail. */
#define B2199_SOFT_MAX  3

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is hard-fail (negative or above soft band). */
static int
b2199_hard_fail_p(int nCode)
{
	if (nCode < 0) {
		return 1;
	}
	if (nCode > B2199_SOFT_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_hard_fail_p - 1 iff smoke code is a hard failure.
 *
 * code: integer smoke result code (signed so negatives hard-fail).
 * Soft band [0, 3] is not hard fail (includes PASS and soft-skip).
 * Codes < 0 or > 3 are hard fails. Does not call libc.
 */
int
gj_smoke_hard_fail_p(int nCode)
{
	(void)NULL;
	return b2199_hard_fail_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_hard_fail_p(int nCode)
    __attribute__((alias("gj_smoke_hard_fail_p")));
