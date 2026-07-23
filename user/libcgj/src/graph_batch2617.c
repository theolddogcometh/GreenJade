/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2617: freestanding n-octet character search (_n_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_chr_n_u(const void *s, int c, size_t n);
 *     - Search the first n octets of s for the first occurrence of
 *       (unsigned char)c. Returns a pointer to that octet, or NULL if
 *       not found. s == NULL or n == 0 -> NULL.
 *   void *__gj_mem_chr_n_u  (alias)
 *   __libcgj_batch2617_marker = "libcgj-batch2617"
 *
 * Post-2610 mem exclusive wave (2611-2620). Distinct from gj_mem_chr
 * (batch400) / gj_mem_chr_n (batch635) / libc memchr - unique
 * gj_mem_chr_n_u surface only; no multi-def. Return type is void *
 * (mutable view into the input window).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2617_marker[] = "libcgj-batch2617";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pointer to first octet equal to ch in pU[0..n), or NULL. */
static unsigned char *
b2617_chr(const unsigned char *pU, unsigned char ch, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] == ch) {
			return (unsigned char *)(void *)(pU + i);
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_chr_n_u - find first octet equal to (unsigned char)c in s[0..n).
 *
 * s: region of at least n readable bytes when n > 0 (NULL -> NULL)
 * c: search byte (truncated to unsigned char)
 * n: number of octets to scan (0 -> NULL)
 *
 * Does not call libc memchr. Returns a pointer into s, or NULL.
 */
void *
gj_mem_chr_n_u(const void *s, int c, size_t n)
{
	(void)NULL;
	if (s == NULL || n == 0u) {
		return NULL;
	}
	return (void *)b2617_chr((const unsigned char *)s,
	    (unsigned char)c, n);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_chr_n_u(const void *s, int c, size_t n)
    __attribute__((alias("gj_mem_chr_n_u")));
