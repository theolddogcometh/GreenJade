/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2606: int64_t max (exclusive _u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_max_u(int64_t a, int64_t b);
 *     - Greater of a and b (equal → b).
 *   int64_t __gj_i64_max_u  (alias)
 *   __libcgj_batch2606_marker = "libcgj-batch2606"
 *
 * Distinct from gj_max_i64 (batch376) — exclusive _u name surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2606_marker[] = "libcgj-batch2606";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b2606_max(int64_t i64A, int64_t i64B)
{
	if (i64A > i64B) {
		return i64A;
	}
	return i64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_max_u — return the greater of two int64_t values.
 *
 * a, b: operands
 * Returns the larger value; when equal, returns b.
 */
int64_t
gj_i64_max_u(int64_t i64A, int64_t i64B)
{
	(void)NULL;
	return b2606_max(i64A, i64B);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_max_u(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_max_u")));
