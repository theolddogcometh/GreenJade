/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3578: freestanding u32 linear scan (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_scan_u32_u(const uint32_t *p, size_t n, uint32_t v);
 *     - Return the least index i with p[i] == v, or n if not found
 *       (also when p == NULL or n == 0). n is element count of u32 words.
 *   size_t __gj_mem_scan_u32_u  (alias)
 *   __libcgj_batch3578_marker = "libcgj-batch3578"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_scan_u32_u surface only; no multi-def. Distinct from
 * gj_mem_scan_eq_u32 (batch2211) / gj_find_u32 (batch403). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3578_marker[] = "libcgj-batch3578";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index of needle in pWords[0..cWords), or cWords if absent. */
static size_t
b3578_scan(const uint32_t *pWords, size_t cWords, uint32_t u32Needle)
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
 * gj_mem_scan_u32_u - first index of v in p[0..n), or n if absent.
 *
 * p: array of n uint32_t words (NULL with any n -> n)
 * n: element count (not byte count)
 * v: needle word
 *
 * Does not call libc. Early-exits on the first match.
 * No parent wires.
 */
size_t
gj_mem_scan_u32_u(const uint32_t *p, size_t n, uint32_t v)
{
	(void)NULL;
	if (p == NULL) {
		return n;
	}
	if (n == 0u) {
		return 0u;
	}
	return b3578_scan(p, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_scan_u32_u(const uint32_t *p, size_t n, uint32_t v)
    __attribute__((alias("gj_mem_scan_u32_u")));
