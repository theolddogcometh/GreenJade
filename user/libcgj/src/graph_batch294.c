/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch294: constant-time buffer equality.
 *
 * Surface (unique symbols — distinct from batch120 gj_memeq_ct,
 * batch41 crypto_memcmp, batch28 consttime_memequal, batch24
 * timingsafe_bcmp):
 *   int gj_ct_eq(const void *a, const void *b, size_t n);
 *     — 1 if equal, 0 if not; no data-dependent early exit.
 *       n == 0 → equal (1). NULL with n > 0 → not equal (0).
 *       Both NULL with n == 0 → equal (1).
 *   __gj_ct_eq  (alias)
 *   __libcgj_batch294_marker = "libcgj-batch294"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch294_marker[] = "libcgj-batch294";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ct_eq — constant-time equality of n bytes at a and b.
 *
 * Walks the full length and accumulates XOR differences so compare time
 * does not depend on the first mismatched byte. Result is normalized to
 * 1 (all bytes match, or n == 0) or 0 (any difference / invalid NULL).
 *
 * NULL handling:
 *   n == 0                 → 1 (empty buffers are equal; pointers unused)
 *   a == NULL || b == NULL → 0 when n > 0
 */
int
gj_ct_eq(const void *a, const void *b, size_t n)
{
	const unsigned char *pA;
	const unsigned char *pB;
	unsigned char uDiff;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}

	pA = (const unsigned char *)a;
	pB = (const unsigned char *)b;
	uDiff = 0u;
	for (i = 0u; i < n; i++) {
		uDiff = (unsigned char)(uDiff | (pA[i] ^ pB[i]));
	}

	/*
	 * Fold accumulated difference to 1 (equal) / 0 (not) without a
	 * data-dependent branch on content. uDiff == 0 →
	 * ((0u - 1u) >> 8) & 1 = 1; nonzero → high bits clear → 0.
	 */
	return (int)((((unsigned)uDiff - 1u) >> 8) & 1u);
}

int __gj_ct_eq(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_ct_eq")));
