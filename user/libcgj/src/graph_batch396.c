/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch396: freestanding memory fill (octet set).
 *
 * Surface (unique symbols):
 *   void *gj_mem_set(void *p, int c, size_t n);
 *     — Fill n octets at p with (unsigned char)c. Returns p.
 *       p == NULL → return NULL (no write). n == 0 → return p unchanged.
 *   void *__gj_mem_set  (alias)
 *   __libcgj_batch396_marker = "libcgj-batch396"
 *
 * Distinct from libc memset in string.c — unique gj_ names only; no
 * multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch396_marker[] = "libcgj-batch396";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_set — fill n octets at p with byte value c.
 *
 * p: destination region of at least n bytes when n > 0 (NULL → NULL)
 * c: fill byte (truncated to unsigned char)
 * n: number of octets to write (0 → no-op, return p)
 */
void *
gj_mem_set(void *p, int c, size_t n)
{
	unsigned char *pb;
	unsigned char ch;
	size_t i;

	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}

	pb = (unsigned char *)p;
	ch = (unsigned char)c;
	for (i = 0u; i < n; i++) {
		pb[i] = ch;
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_set(void *p, int c, size_t n)
    __attribute__((alias("gj_mem_set")));
