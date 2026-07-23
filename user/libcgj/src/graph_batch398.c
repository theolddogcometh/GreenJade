/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch398: freestanding overlapping-safe memory move.
 *
 * Surface (unique symbols):
 *   void *gj_mem_move(void *d, const void *s, size_t n);
 *     — Copy n octets from s to d, correct for overlapping regions.
 *       Returns d. d == NULL or s == NULL → return d. n == 0 → d.
 *       If d < s, copy forward; if d > s, copy backward.
 *   void *__gj_mem_move  (alias)
 *   __libcgj_batch398_marker = "libcgj-batch398"
 *
 * Distinct from libc memmove in string.c — unique gj_ names only; no
 * multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch398_marker[] = "libcgj-batch398";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_move — overlap-safe octet copy from s to d.
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to move
 *
 * Does not call libc memmove. When d == s, no bytes are written.
 */
void *
gj_mem_move(void *d, const void *s, size_t n)
{
	unsigned char *pd;
	const unsigned char *ps;
	size_t i;

	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u || d == s) {
		return d;
	}

	pd = (unsigned char *)d;
	ps = (const unsigned char *)s;

	if (pd < ps) {
		for (i = 0u; i < n; i++) {
			pd[i] = ps[i];
		}
	} else {
		i = n;
		while (i > 0u) {
			i--;
			pd[i] = ps[i];
		}
	}
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_move(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_move")));
