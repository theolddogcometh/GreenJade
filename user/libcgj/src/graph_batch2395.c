/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2395: product smoke red (FAIL) predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_red_p(int code);
 *     - Return 1 if code is red FAIL (any non-zero), else 0.
 *       Soft pure-data gate complementary to gj_smoke_green_p.
 *   int __gj_smoke_red_p  (alias)
 *   __libcgj_batch2395_marker = "libcgj-batch2395"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_smoke_red_p surface only; no multi-def. Distinct from
 * gj_smoke_green_p (batch2394) and gj_smoke_gate_hard_p (batch1996).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2395_marker[] = "libcgj-batch2395";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is non-zero (red FAIL lamp). */
static int
b2395_red_p(int nCode)
{
	if (nCode != 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_red_p - 1 iff smoke code is red FAIL (non-zero).
 *
 * code: integer smoke result code (signed so negatives are red).
 * Red is any code != 0. Pair with gj_smoke_green_p for the complementary
 * pass lamp. Does not call libc.
 */
int
gj_smoke_red_p(int nCode)
{
	(void)NULL;
	return b2395_red_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_red_p(int nCode)
    __attribute__((alias("gj_smoke_red_p")));
