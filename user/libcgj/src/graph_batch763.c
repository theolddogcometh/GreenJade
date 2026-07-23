/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch763: freestanding bounded memmem-style contains.
 *
 * Surface (unique symbols):
 *   int gj_mem_contains(const void *hay, size_t n,
 *                       const void *needle, size_t np);
 *     — Return 1 if the np-octet needle appears anywhere in hay[0..n),
 *       else 0. np == 0 is a vacuous match (returns 1). If np > 0 and
 *       either pointer is NULL, or if np > n, returns 0.
 *   int __gj_mem_contains  (alias)
 *   __libcgj_batch763_marker = "libcgj-batch763"
 *
 * Distinct from gj_mem_starts_with / gj_mem_eq / gj_mem_cmp — unique
 * gj_mem_contains surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc memmem. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch763_marker[] = "libcgj-batch763";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_contains — 1 if needle[0..np) is found in hay[0..n).
 *
 * hay:    haystack of at least n bytes when n > 0 and np > 0
 * n:      haystack length in octets
 * needle: pattern of at least np bytes when np > 0
 * np:     needle length (0 → 1, vacuous match)
 *
 * Naive window scan; early-exits on first full match. Unsigned octets.
 */
int
gj_mem_contains(const void *pHay, size_t n, const void *pNeedle, size_t np)
{
	const unsigned char *pH;
	const unsigned char *pN;
	size_t iBase;
	size_t i;
	size_t cMax;

	if (np == 0u) {
		return 1;
	}
	if (np > n) {
		return 0;
	}
	if (pHay == NULL || pNeedle == NULL) {
		return 0;
	}

	pH = (const unsigned char *)pHay;
	pN = (const unsigned char *)pNeedle;
	cMax = n - np;
	for (iBase = 0u; iBase <= cMax; iBase++) {
		for (i = 0u; i < np; i++) {
			if (pH[iBase + i] != pN[i]) {
				break;
			}
		}
		if (i == np) {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_contains(const void *pHay, size_t n, const void *pNeedle,
    size_t np)
    __attribute__((alias("gj_mem_contains")));
