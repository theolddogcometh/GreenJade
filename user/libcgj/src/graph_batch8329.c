/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8329: qsort/bsearch raw-byte three-way cmp.
 *
 * Surface (unique symbols):
 *   int gj_cmp_bytes_u_8329(const void *a, const void *b, size_t n);
 *     - Lexicographic unsigned-byte compare of n octets at a and b.
 *       Returns -1 / 0 / +1. NULL a or b with n > 0 treats missing side
 *       as less (NULL < non-NULL); n == 0 → 0. Soft memcmp-style helper
 *       for opaque qsort keys.
 *   int __gj_cmp_bytes_u_8329  (alias)
 *   __libcgj_batch8329_marker = "libcgj-batch8329"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Distinct from memcmp / gj_memcmp_sse2 — unique _u_8329
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8329_marker[] = "libcgj-batch8329";

/* ---- freestanding helpers ---------------------------------------------- */

/* Lexicographic unsigned octet compare of cN bytes. */
static int
b8329_cmp(const void *pA, const void *pB, size_t cN)
{
	const unsigned char *pX;
	const unsigned char *pY;
	size_t i;

	if (cN == 0u) {
		return 0;
	}
	if (pA == NULL && pB == NULL) {
		return 0;
	}
	if (pA == NULL) {
		return -1;
	}
	if (pB == NULL) {
		return 1;
	}

	pX = (const unsigned char *)pA;
	pY = (const unsigned char *)pB;
	for (i = 0u; i < cN; i++) {
		if (pX[i] < pY[i]) {
			return -1;
		}
		if (pX[i] > pY[i]) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmp_bytes_u_8329 - three-way raw-byte compare of n octets.
 *
 * a, b: buffers (may be NULL when n == 0, or for soft NULL ordering)
 * n:    octet count
 *
 * Returns -1 / 0 / +1. No parent wires.
 */
int
gj_cmp_bytes_u_8329(const void *pA, const void *pB, size_t cN)
{
	(void)NULL;
	return b8329_cmp(pA, pB, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cmp_bytes_u_8329(const void *pA, const void *pB, size_t cN)
    __attribute__((alias("gj_cmp_bytes_u_8329")));
