/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1566: common prefix length of two C strings.
 *
 * NOTE: gj_str_common_prefix_len already exists in graph_batch1043.c.
 * This TU exports a distinct n-bounded surface to stay unique at link:
 *
 * Surface (unique symbols):
 *   size_t gj_str_common_prefix_n(const char *a, const char *b, size_t n);
 *     — Shared prefix length of a and b, capped at n octets. Stops at
 *       first mismatch, either NUL, or n. NULL a/b → 0.
 *   size_t __gj_str_common_prefix_n  (alias)
 *   __libcgj_batch1566_marker = "libcgj-batch1566"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1566_marker[] = "libcgj-batch1566";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_common_prefix_n — shared prefix length, at most n.
 *
 * Theme label was gj_str_common_prefix_len (taken by batch1043); this is
 * the unique n-bounded counterpart for string-metrics batch1566.
 */
size_t
gj_str_common_prefix_n(const char *a, const char *b, size_t n)
{
	size_t i;

	if (a == NULL || b == NULL || n == 0u) {
		return 0u;
	}

	i = 0u;
	while (i < n && a[i] != '\0' && b[i] != '\0' &&
	    (unsigned char)a[i] == (unsigned char)b[i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_common_prefix_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_common_prefix_n")));
