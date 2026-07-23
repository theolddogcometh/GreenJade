/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2211: freestanding u32 equality linear scan.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_scan_eq_u32(const uint32_t *p, size_t n, uint32_t v);
 *     - Return the least index i with p[i] == v, or n if not found
 *       (also when p == NULL or n == 0). n is element count of u32 words.
 *   size_t __gj_mem_scan_eq_u32  (alias)
 *   __libcgj_batch2211_marker = "libcgj-batch2211"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from gj_find_u32
 * (batch403) - unique gj_mem_scan_eq_u32 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2211_marker[] = "libcgj-batch2211";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index of u32Needle in pWords[0..cWords), or cWords if absent. */
static size_t
b2211_scan_eq(const uint32_t *pWords, size_t cWords, uint32_t u32Needle)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		if (pWords[iWord] == u32Needle) {
			return iWord;
		}
	}
	return cWords;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_scan_eq_u32 - first index of v in p[0..n), or n if absent.
 *
 * p: array of n uint32_t words (NULL with any n -> n)
 * n: element count (not byte count)
 * v: needle word
 *
 * Does not call libc. Early-exits on the first match.
 */
size_t
gj_mem_scan_eq_u32(const uint32_t *p, size_t n, uint32_t v)
{
	(void)NULL;
	if (p == NULL) {
		return n;
	}
	if (n == 0u) {
		return 0u;
	}
	return b2211_scan_eq(p, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_scan_eq_u32(const uint32_t *p, size_t n, uint32_t v)
    __attribute__((alias("gj_mem_scan_eq_u32")));
