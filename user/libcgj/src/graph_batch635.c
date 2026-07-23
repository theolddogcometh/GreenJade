/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch635: freestanding memory character search (n-first).
 *
 * Surface (unique symbols):
 *   const void *gj_mem_chr_n(const void *p, size_t n, int c);
 *     — Search the first n octets of p for the first occurrence of
 *       (unsigned char)c. Returns a pointer to that octet, or NULL if
 *       not found. p == NULL or n == 0 → NULL.
 *   const void *__gj_mem_chr_n  (alias)
 *   __libcgj_batch635_marker = "libcgj-batch635"
 *
 * Distinct from libc memchr and gj_mem_chr (batch400) — unique gj_ names
 * only; argument order is (p, n, c) rather than (s, c, n). Return type is
 * const void * (read-only view of the input window).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch635_marker[] = "libcgj-batch635";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_chr_n — find first octet equal to (unsigned char)c in p[0..n).
 *
 * p: region of at least n readable bytes when n > 0 (NULL → NULL)
 * n: number of octets to scan (0 → NULL)
 * c: search byte (truncated to unsigned char)
 *
 * Does not call libc memchr or gj_mem_chr. Returns const view into p,
 * or NULL.
 */
const void *
gj_mem_chr_n(const void *p, size_t n, int c)
{
	const unsigned char *pb;
	unsigned char ch;
	size_t i;

	if (p == NULL || n == 0u) {
		return NULL;
	}

	pb = (const unsigned char *)p;
	ch = (unsigned char)c;
	for (i = 0u; i < n; i++) {
		if (pb[i] == ch) {
			return (const void *)(pb + i);
		}
	}
	return NULL;
}

/* ---- underscored alias ------------------------------------------------- */

const void *__gj_mem_chr_n(const void *p, size_t n, int c)
    __attribute__((alias("gj_mem_chr_n")));
