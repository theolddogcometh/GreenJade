/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1569: common suffix length of two memory windows.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_common_suffix_len(const void *a, const void *b, size_t n);
 *     — Byte length of the longest shared suffix of a[0..n) and b[0..n).
 *       Comparison is unsigned-octet ordinal. NULL a/b or n==0 → 0.
 *   size_t __gj_mem_common_suffix_len  (alias)
 *   __libcgj_batch1569_marker = "libcgj-batch1569"
 *
 * Distinct from NUL-string gj_str_common_suffix_len (batch1044) and
 * gj_str_common_suffix_n (batch1567).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1569_marker[] = "libcgj-batch1569";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_common_suffix_len — shared suffix of two n-octet buffers.
 */
size_t
gj_mem_common_suffix_len(const void *a, const void *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (a == NULL || b == NULL || n == 0u) {
		return 0u;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	i = 0u;
	while (i < n && pa[n - 1u - i] == pb[n - 1u - i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_common_suffix_len(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_common_suffix_len")));
