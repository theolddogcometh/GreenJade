/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch648: checked uint64_t subtraction (0 / -1).
 *
 * Surface (unique symbols):
 *   int gj_u64_sub_check(uint64_t a, uint64_t b, uint64_t *out);
 *     — Compute a - b. On success write the difference to *out and
 *       return 0. On underflow (a < b) leave *out untouched and
 *       return -1. out == NULL → return -1 (no write).
 *   int __gj_u64_sub_check  (alias)
 *   __libcgj_batch648_marker = "libcgj-batch648"
 *
 * Does NOT define gj_sat_sub_u64 / __gj_sat_sub_u64 (batch377) —
 * that saturates to 0; this reports failure. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch648_marker[] = "libcgj-batch648";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_check — subtract a - b with underflow check.
 *
 * a, b: minuend and subtrahend
 * out:  result pointer (required; NULL → -1)
 *
 * Returns 0 if a >= b and the difference was stored at *out;
 * returns -1 on underflow or NULL out (no store on underflow).
 */
int
gj_u64_sub_check(uint64_t a, uint64_t b, uint64_t *out)
{
	uint64_t u64Diff;

	if (out == NULL) {
		return -1;
	}

	/* Underflow iff a < b. */
	if (a < b) {
		return -1;
	}

	u64Diff = a - b;
	*out = u64Diff;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_sub_check(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_sub_check")));
