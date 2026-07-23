/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch646: checked uint64 multiply (OK/FAIL).
 *
 * Surface (unique symbols):
 *   int gj_u64_mul_check(uint64_t a, uint64_t b, uint64_t *out);
 *     — Compute a * b if the product fits in uint64_t.
 *       Return 0 and write the product to *out when out is non-NULL.
 *       Return -1 on overflow (*out left untouched).
 *       out may be NULL (status-only probe; no write on OK or FAIL).
 *   int __gj_u64_mul_check  (alias)
 *   __libcgj_batch646_marker = "libcgj-batch646"
 *
 * Distinct from gj_mul_overflow_u64 / __gj_mul_overflow_u64 (batch428),
 * which returns 0/1 and always stores a wrapping product when out is
 * non-NULL. Do not multi-def those symbols here.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch646_marker[] = "libcgj-batch646";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_check — multiply a * b with overflow check.
 *
 * a, b: factors
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 0 if a * b fits in uint64_t (writes product to *out when
 * out is non-NULL). Returns -1 on overflow (*out unchanged).
 */
int
gj_u64_mul_check(uint64_t a, uint64_t b, uint64_t *out)
{
	/* Zero factor never overflows. */
	if (a == 0u || b == 0u) {
		if (out != NULL) {
			*out = 0u;
		}
		return 0;
	}

	/* a * b overflows iff a > floor(UINT64_MAX / b). */
	if (a > (UINT64_C(0xffffffffffffffff) / b)) {
		return -1;
	}

	if (out != NULL) {
		*out = a * b;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_mul_check(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_mul_check")));
