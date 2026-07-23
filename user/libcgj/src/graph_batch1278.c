/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1278: constant-time u8 buffer compare.
 *
 * Surface (unique symbols — distinct from crypto_memcmp (batch41),
 * gj_ct_eq (batch294), consttime_memequal (batch28)):
 *   int gj_crypto_memcmp_u8(const unsigned char *a,
 *                           const unsigned char *b, size_t n);
 *     — Constant-time compare of n bytes. Returns 0 if equal,
 *       nonzero (1) if not equal. n==0 → 0 (equal). NULL with n>0 → 1.
 *   int __gj_crypto_memcmp_u8  (alias)
 *   __libcgj_batch1278_marker = "libcgj-batch1278"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1278_marker[] = "libcgj-batch1278";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_memcmp_u8 — constant-time bytewise compare; 0 if equal.
 *
 * Accumulates XOR differences over the full length so runtime does not
 * depend on the first mismatched byte. Result is normalized to 0/1.
 *
 * NULL handling:
 *   n == 0                 → 0 (empty equal; pointers unused)
 *   a == NULL || b == NULL → 1 when n > 0
 */
int
gj_crypto_memcmp_u8(const unsigned char *pA, const unsigned char *pB,
                    size_t cbN)
{
	unsigned char uDiff;
	size_t i;

	if (cbN == 0u) {
		return 0;
	}
	if (pA == NULL || pB == NULL) {
		return 1;
	}

	uDiff = 0u;
	for (i = 0u; i < cbN; i++) {
		uDiff = (unsigned char)(uDiff | (pA[i] ^ pB[i]));
	}

	/* Normalize: 0 if equal, 1 if any difference. */
	return (uDiff != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_crypto_memcmp_u8(const unsigned char *pA, const unsigned char *pB,
                          size_t cbN)
    __attribute__((alias("gj_crypto_memcmp_u8")));
