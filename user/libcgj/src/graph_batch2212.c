/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2212: freestanding u64 equality linear scan.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_scan_eq_u64(const uint64_t *p, size_t n, uint64_t v);
 *     - Return the least index i with p[i] == v, or n if not found
 *       (also when p == NULL or n == 0). n is element count of u64 words.
 *   size_t __gj_mem_scan_eq_u64  (alias)
 *   __libcgj_batch2212_marker = "libcgj-batch2212"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from
 * gj_mem_scan_eq_u32 (batch2211) / gj_find_u32 - unique
 * gj_mem_scan_eq_u64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2212_marker[] = "libcgj-batch2212";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index of u64Needle in pWords[0..cWords), or cWords if absent. */
static size_t
b2212_scan_eq(const uint64_t *pWords, size_t cWords, uint64_t u64Needle)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		if (pWords[iWord] == u64Needle) {
			return iWord;
		}
	}
	return cWords;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_scan_eq_u64 - first index of v in p[0..n), or n if absent.
 *
 * p: array of n uint64_t words (NULL with any n -> n)
 * n: element count (not byte count)
 * v: needle word
 *
 * Does not call libc. Early-exits on the first match.
 */
size_t
gj_mem_scan_eq_u64(const uint64_t *p, size_t n, uint64_t v)
{
	(void)NULL;
	if (p == NULL) {
		return n;
	}
	if (n == 0u) {
		return 0u;
	}
	return b2212_scan_eq(p, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_scan_eq_u64(const uint64_t *p, size_t n, uint64_t v)
    __attribute__((alias("gj_mem_scan_eq_u64")));
