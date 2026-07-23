/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch428: checked uint64 multiply with overflow flag.
 *
 * Surface (unique symbols):
 *   int gj_mul_overflow_u64(uint64_t a, uint64_t b, uint64_t *out);
 *     — Compute a * b. On success write the product to *out and return 0.
 *       On overflow write the wrapping product to *out and return 1.
 *       out == NULL → return 1 (no write).
 *   int __gj_mul_overflow_u64  (alias)
 *   __libcgj_batch428_marker = "libcgj-batch428"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch428_marker[] = "libcgj-batch428";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mul_overflow_u64 — multiply a * b with overflow detection.
 *
 * a, b: factors
 * out:  result pointer (required; NULL → 1)
 *
 * Returns 0 if the mathematical product fits in uint64_t, 1 on overflow.
 * *out always receives a * b with unsigned wrap when out is non-NULL.
 */
int
gj_mul_overflow_u64(uint64_t a, uint64_t b, uint64_t *out)
{
	uint64_t u64Prod;
	int nOv;

	if (out == NULL) {
		return 1;
	}

	/* Zero factor never overflows. */
	if (a == 0u || b == 0u) {
		*out = 0u;
		return 0;
	}

	/* a * b overflows iff a > floor(UINT64_MAX / b). */
	nOv = (a > (UINT64_C(0xffffffffffffffff) / b)) ? 1 : 0;
	u64Prod = a * b; /* wrap defined for unsigned */
	*out = u64Prod;
	return nOv;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mul_overflow_u64(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_mul_overflow_u64")));
