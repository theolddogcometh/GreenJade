/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1561: character Hamming distance over n octets.
 *
 * Surface (unique symbols):
 *   size_t gj_str_hamming_n(const char *a, const char *b, size_t n);
 *     — Count of positions i in [0, n) where a[i] != b[i] (unsigned
 *       char ordinal). Treats a and b as equal-length n-octet windows
 *       (not necessarily NUL-terminated). NULL a or NULL b → 0.
 *       Distinct from bit-level gj_hamming_distance (batch184) and
 *       equal-C-string gj_hamming_str (batch213).
 *   size_t __gj_str_hamming_n  (alias)
 *   __libcgj_batch1561_marker = "libcgj-batch1561"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1561_marker[] = "libcgj-batch1561";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hamming_n — character Hamming distance of two n-octet windows.
 */
size_t
gj_str_hamming_n(const char *a, const char *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;
	size_t dist;

	if (a == NULL || b == NULL || n == 0u) {
		return 0u;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	dist = 0u;
	for (i = 0u; i < n; i++) {
		if (pa[i] != pb[i]) {
			dist++;
		}
	}
	return dist;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_hamming_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_hamming_n")));
