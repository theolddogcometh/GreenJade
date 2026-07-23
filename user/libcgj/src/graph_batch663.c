/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch663: freestanding pairwise memory swap.
 *
 * Surface (unique symbols):
 *   void gj_mem_swap(void *a, void *b, size_t n);
 *     — Exchange n octets between regions a and b. Any of a, b NULL →
 *       no-op. n == 0 → no-op. Same pointer a == b → no-op.
 *   void __gj_mem_swap  (alias)
 *   __libcgj_batch663_marker = "libcgj-batch663"
 *
 * Distinct from libc mem* and from gj_mem_move / gj_mem_cpy — unique
 * gj_ names only; no multi-def with the existing string or mem graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch663_marker[] = "libcgj-batch663";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_swap — swap n octets between a and b.
 *
 * a: first region of at least n bytes when n > 0 (NULL → no-op)
 * b: second region of at least n bytes when n > 0 (NULL → no-op)
 * n: number of octets to exchange (0 → no-op)
 *
 * Overlap between a and b other than a == b is not supported (per-octet
 * temp walk). When a == b the regions are left unchanged.
 */
void
gj_mem_swap(void *a, void *b, size_t n)
{
	unsigned char *pa;
	unsigned char *pb;
	unsigned char t;
	size_t i;

	if (a == NULL || b == NULL) {
		return;
	}
	if (n == 0u) {
		return;
	}
	if (a == b) {
		return;
	}

	pa = (unsigned char *)a;
	pb = (unsigned char *)b;
	for (i = 0u; i < n; i++) {
		t = pa[i];
		pa[i] = pb[i];
		pb[i] = t;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mem_swap(void *a, void *b, size_t n)
    __attribute__((alias("gj_mem_swap")));
