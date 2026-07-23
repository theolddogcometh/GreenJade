/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1514: constant-time memory equality (0/1).
 *
 * Surface (unique symbols):
 *   int gj_ct_eq_mem_n(const void *a, const void *b, size_t n);
 *     — 1 if the n-byte regions are equal, else 0. Always walks full
 *       length (no data-dependent early exit). n == 0 → 1. NULL with
 *       n > 0 → 0.
 *   int __gj_ct_eq_mem_n  (alias)
 *   __libcgj_batch1514_marker = "libcgj-batch1514"
 *
 * Distinct from gj_ct_eq (batch294), gj_memeq_ct (batch120),
 * crypto_memcmp (batch41), consttime_memequal (batch28),
 * timingsafe_bcmp (batch24). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No real crypto copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1514_marker[] = "libcgj-batch1514";

/*
 * gj_ct_eq_mem_n — constant-time equality of n bytes at a and b.
 *
 * Accumulates XOR differences over the full span so compare time does
 * not depend on the first mismatched byte. Result is 1 (equal / n==0)
 * or 0 (difference / invalid NULL).
 */
int
gj_ct_eq_mem_n(const void *pA, const void *pB, size_t cbN)
{
	const unsigned char *pX;
	const unsigned char *pY;
	unsigned char uDiff;
	size_t i;

	if (cbN == 0u) {
		return 1;
	}
	if (pA == NULL || pB == NULL) {
		return 0;
	}

	pX = (const unsigned char *)pA;
	pY = (const unsigned char *)pB;
	uDiff = 0u;
	for (i = 0u; i < cbN; i++) {
		uDiff = (unsigned char)(uDiff | (pX[i] ^ pY[i]));
	}

	/*
	 * uDiff == 0 → ((0u - 1u) >> 8) & 1 = 1;
	 * nonzero    → high bits clear → 0.
	 */
	return (int)((((unsigned)uDiff - 1u) >> 8) & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ct_eq_mem_n(const void *pA, const void *pB, size_t cbN)
    __attribute__((alias("gj_ct_eq_mem_n")));
