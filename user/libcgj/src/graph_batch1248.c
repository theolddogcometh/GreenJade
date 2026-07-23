/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1248: constant-time byte-buffer equality.
 *
 * Surface (unique symbols):
 *   int gj_bytes_eq_consttime(const void *a, const void *b, size_t n);
 *     — 1 if equal, 0 if not; no data-dependent early exit.
 *       n==0 → 1. NULL with n>0 → 0.
 *   int __gj_bytes_eq_consttime  (alias)
 *   __libcgj_batch1248_marker = "libcgj-batch1248"
 *
 * Distinct from gj_ct_eq / gj_memeq_ct / consttime_memequal — unique name.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1248_marker[] = "libcgj-batch1248";

/*
 * gj_bytes_eq_consttime — constant-time equality of n octets.
 */
int
gj_bytes_eq_consttime(const void *pA, const void *pB, size_t cb)
{
	const unsigned char *pX;
	const unsigned char *pY;
	unsigned char uDiff;
	size_t i;

	if (cb == 0u) {
		return 1;
	}
	if (pA == NULL || pB == NULL) {
		return 0;
	}

	pX = (const unsigned char *)pA;
	pY = (const unsigned char *)pB;
	uDiff = 0u;
	for (i = 0u; i < cb; i++) {
		uDiff = (unsigned char)(uDiff | (pX[i] ^ pY[i]));
	}

	/* uDiff==0 → 1; nonzero → 0 (no data-dependent branch on content). */
	return (int)((((unsigned)uDiff - 1u) >> 8) & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bytes_eq_consttime(const void *pA, const void *pB, size_t cb)
    __attribute__((alias("gj_bytes_eq_consttime")));
