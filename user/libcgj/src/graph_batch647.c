/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch647: checked uint64_t addition (0 / -1).
 *
 * Surface (unique symbols):
 *   int gj_u64_add_check(uint64_t a, uint64_t b, uint64_t *out);
 *     — Compute a + b. On success write the sum to *out and return 0.
 *       On overflow leave *out untouched and return -1.
 *       out == NULL → return -1 (no write).
 *   int __gj_u64_add_check  (alias)
 *   __libcgj_batch647_marker = "libcgj-batch647"
 *
 * Does NOT define gj_add_overflow_u64 / __gj_add_overflow_u64 (batch427)
 * or gj_sat_add_u64 (batch377) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch647_marker[] = "libcgj-batch647";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_add_check — add a + b with overflow check.
 *
 * a, b: addends
 * out:  result pointer (required; NULL → -1)
 *
 * Returns 0 if the mathematical sum fits in uint64_t and was stored at
 * *out; returns -1 on overflow or NULL out (no store on overflow).
 */
int
gj_u64_add_check(uint64_t a, uint64_t b, uint64_t *out)
{
	uint64_t u64Sum;

	if (out == NULL) {
		return -1;
	}

	/* Overflow iff b > UINT64_MAX - a (equivalently sum wraps below a). */
	if (b > (UINT64_MAX - a)) {
		return -1;
	}

	u64Sum = a + b;
	*out = u64Sum;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_add_check(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_add_check")));
