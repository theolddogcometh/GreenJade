/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch528: freestanding pairwise memory XOR.
 *
 * Surface (unique symbols):
 *   void gj_mem_xor(void *dst, const void *a, const void *b, size_t n);
 *     — For each i in [0, n): dst[i] = a[i] ^ b[i] (unsigned octets).
 *       Any of dst, a, b NULL → no-op. n == 0 → no-op.
 *   void __gj_mem_xor  (alias)
 *   __libcgj_batch528_marker = "libcgj-batch528"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch528_marker[] = "libcgj-batch528";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_xor — pairwise octet XOR of a and b into dst.
 *
 * dst: destination of at least n bytes when n > 0 (NULL → no-op)
 * a:   first source region (NULL → no-op)
 * b:   second source region (NULL → no-op)
 * n:   number of octets to XOR (0 → no-op)
 *
 * Overlap between dst and a/b is not supported (forward walk only).
 */
void
gj_mem_xor(void *dst, const void *a, const void *b, size_t n)
{
	unsigned char *pd;
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (dst == NULL || a == NULL || b == NULL) {
		return;
	}
	if (n == 0u) {
		return;
	}

	pd = (unsigned char *)dst;
	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	for (i = 0u; i < n; i++) {
		pd[i] = (unsigned char)(pa[i] ^ pb[i]);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mem_xor(void *dst, const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_xor")));
