/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4537: size_t multiplication overflow predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_size_mul_fits_u(size_t a, size_t b);
 *     - Return 1 if a * b is representable as size_t (no overflow),
 *       else 0. Zero factors always fit.
 *   int __gj_size_mul_fits_u  (alias)
 *   __libcgj_batch4537_marker = "libcgj-batch4537"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_size_mul_fits_u surface only; no multi-def. Distinct from
 * gj_size_add_fits_u (batch4536). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4537_marker[] = "libcgj-batch4537";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if a * b does not overflow size_t, else 0. */
static int
b4537_mul_fits(size_t uA, size_t uB)
{
	if (uA == 0u || uB == 0u) {
		return 1;
	}
	if (uA > (SIZE_MAX / uB)) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_size_mul_fits_u - 1 if a * b fits in size_t, else 0.
 *
 * a: first factor
 * b: second factor
 *
 * Zero either side → 1 (product is 0). Does not compute the product.
 * No parent wires.
 */
int
gj_size_mul_fits_u(size_t uA, size_t uB)
{
	(void)NULL;
	return b4537_mul_fits(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_size_mul_fits_u(size_t uA, size_t uB)
    __attribute__((alias("gj_size_mul_fits_u")));
