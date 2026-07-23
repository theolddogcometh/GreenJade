/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch427: checked uint64 addition with overflow flag.
 *
 * Surface (unique symbols):
 *   int gj_add_overflow_u64(uint64_t a, uint64_t b, uint64_t *out);
 *     — Compute a + b. On success write the sum to *out and return 0.
 *       On overflow write the wrapping sum to *out and return 1.
 *       out == NULL → return 1 (no write).
 *   int __gj_add_overflow_u64  (alias)
 *   __libcgj_batch427_marker = "libcgj-batch427"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch427_marker[] = "libcgj-batch427";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_add_overflow_u64 — add a + b with overflow detection.
 *
 * a, b: addends
 * out:  result pointer (required; NULL → 1)
 *
 * Returns 0 if the mathematical sum fits in uint64_t, 1 on overflow.
 * *out always receives a + b with unsigned wrap when out is non-NULL.
 */
int
gj_add_overflow_u64(uint64_t a, uint64_t b, uint64_t *out)
{
	uint64_t u64Sum;

	if (out == NULL) {
		return 1;
	}

	u64Sum = a + b;
	*out = u64Sum;

	/* Overflow iff a > UINT64_MAX - b (equivalently sum < a). */
	if (u64Sum < a) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_add_overflow_u64(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_add_overflow_u64")));
