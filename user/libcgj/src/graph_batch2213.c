/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2213: freestanding first unequal-octet scan.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_scan_neq_u8(const void *p, size_t n, unsigned char v);
 *     - Return the least byte index i with p[i] != v, or n if every
 *       octet equals v (also when p == NULL with n > 0, or n == 0).
 *   size_t __gj_mem_scan_neq_u8  (alias)
 *   __libcgj_batch2213_marker = "libcgj-batch2213"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from gj_mem_chr /
 * gj_mem_chr_n / gj_mem_eq_byte_n - unique gj_mem_scan_neq_u8 surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2213_marker[] = "libcgj-batch2213";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index with pU[i] != uNeedle, or cb if all equal. */
static size_t
b2213_scan_neq(const unsigned char *pU, size_t cb, unsigned char uNeedle)
{
	size_t iByte;

	for (iByte = 0u; iByte < cb; iByte++) {
		if (pU[iByte] != uNeedle) {
			return iByte;
		}
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_scan_neq_u8 - first index of an octet not equal to v in p[0..n).
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> n)
 * n: number of octets to scan (0 -> 0)
 * v: reference fill value; first mismatch wins
 *
 * Does not call libc. Early-exits on the first unequal octet.
 */
size_t
gj_mem_scan_neq_u8(const void *p, size_t n, unsigned char v)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (p == NULL) {
		return n;
	}
	return b2213_scan_neq((const unsigned char *)p, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_scan_neq_u8(const void *p, size_t n, unsigned char v)
    __attribute__((alias("gj_mem_scan_neq_u8")));
