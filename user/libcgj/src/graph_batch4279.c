/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4279: checked size_t add, 1 if ok (_u).
 *
 * Surface (unique symbols):
 *   int gj_size_add_ok_u(size_t a, size_t b, size_t *out);
 *     - Compute a + b. Return 1 if the mathematical sum fits in size_t
 *       (and write the sum to *out when out is non-NULL). Return 0 on
 *       overflow (*out left untouched). out may be NULL (status-only).
 *   int __gj_size_add_ok_u  (alias)
 *   __libcgj_batch4279_marker = "libcgj-batch4279"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_u64_add_overflow_u (batch4274) / gj_u64_add_check
 * (batch647) — unique gj_size_add_ok_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4279_marker[] = "libcgj-batch4279";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 and optional store when a + b fits in size_t; else 0.
 * Overflow iff b > SIZE_MAX - a (equivalently sum wraps below a).
 */
static int
b4279_add_ok(size_t szA, size_t szB, size_t *pOut)
{
	size_t szSum;

	if (szB > (SIZE_MAX - szA)) {
		return 0;
	}
	szSum = szA + szB;
	if (pOut != NULL) {
		*pOut = szSum;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_add_ok_u - add size_t a + b; 1 if ok.
 *
 * a, b: addends
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a + b fits in size_t (writes sum to *out when out is
 * non-NULL). Returns 0 on overflow (*out unchanged). No parent wires.
 */
int
gj_size_add_ok_u(size_t a, size_t b, size_t *out)
{
	(void)NULL;
	return b4279_add_ok(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_size_add_ok_u(size_t a, size_t b, size_t *out)
    __attribute__((alias("gj_size_add_ok_u")));
