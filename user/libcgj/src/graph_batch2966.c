/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2966: freestanding last-octet search (_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_rchr_u(const void *s, int c, size_t n);
 *     - Search the first n octets of s from the end for the last
 *       occurrence of (unsigned char)c. Returns a pointer to that
 *       octet, or NULL if not found. s == NULL or n == 0 -> NULL.
 *   void *__gj_mem_rchr_u  (alias)
 *   __libcgj_batch2966_marker = "libcgj-batch2966"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_rchr_n_u
 * (batch2618) / gj_mem_chr_u (batch2965) / libc memrchr - unique
 * gj_mem_rchr_u surface only; no multi-def. Binary-safe reverse scan
 * over a fixed window (n is the sole bound). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2966_marker[] = "libcgj-batch2966";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pointer to last octet equal to ch in pU[0..n), or NULL. */
static unsigned char *
b2966_rchr(const unsigned char *pU, unsigned char ch, size_t n)
{
	size_t i;

	i = n;
	while (i > 0u) {
		i--;
		if (pU[i] == ch) {
			return (unsigned char *)(void *)(pU + i);
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_rchr_u - find last octet equal to (unsigned char)c in s[0..n).
 *
 * s: region of at least n readable bytes when n > 0 (NULL -> NULL)
 * c: search byte (truncated to unsigned char)
 * n: number of octets to scan (0 -> NULL)
 *
 * Does not call libc. Returns a pointer into s, or NULL.
 */
void *
gj_mem_rchr_u(const void *s, int c, size_t n)
{
	(void)NULL;
	if (s == NULL || n == 0u) {
		return NULL;
	}
	return (void *)b2966_rchr((const unsigned char *)s,
	    (unsigned char)c, n);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_rchr_u(const void *s, int c, size_t n)
    __attribute__((alias("gj_mem_rchr_u")));
