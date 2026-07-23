/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2023: freestanding count of matching octets.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_count_byte_n(const void *p, size_t n, int c);
 *     — Count how many octets in p[0..n) equal (unsigned char)c.
 *       n == 0 → 0 even if p is NULL. If n > 0 and p is NULL → 0.
 *   size_t __gj_mem_count_byte_n  (alias)
 *   __libcgj_batch2023_marker = "libcgj-batch2023"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_chr /
 * gj_mem_chr_n — unique gj_mem_count_byte_n surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2023_marker[] = "libcgj-batch2023";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count octets equal to uNeedle in pU[0..n). Caller: pU non-NULL when n>0. */
static size_t
b2023_count(const unsigned char *pU, size_t n, unsigned char uNeedle)
{
	size_t i;
	size_t cHit;

	cHit = 0u;
	for (i = 0u; i < n; i++) {
		if (pU[i] == uNeedle) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_count_byte_n — count octets equal to (unsigned char)c in p[0..n).
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 0)
 * c: target byte value (cast to unsigned char)
 *
 * Does not call libc.
 */
size_t
gj_mem_count_byte_n(const void *p, size_t n, int c)
{
	unsigned char uNeedle;

	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (p == NULL) {
		return 0u;
	}
	uNeedle = (unsigned char)c;
	return b2023_count((const unsigned char *)p, n, uNeedle);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_count_byte_n(const void *p, size_t n, int c)
    __attribute__((alias("gj_mem_count_byte_n")));
