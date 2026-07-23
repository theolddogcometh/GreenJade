/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2764: freestanding count of matching u8 octets.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_count_u8_u(const void *p, size_t n, uint8_t v);
 *     - Count how many octets in p[0..n) equal v. n == 0 -> 0 even if
 *       p is NULL. If n > 0 and p is NULL -> 0. Binary-safe: n is the
 *       sole bound.
 *   size_t __gj_mem_count_u8_u  (alias)
 *   __libcgj_batch2764_marker = "libcgj-batch2764"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_mem_count_byte_n (batch2023, int c form) - unique uint8_t
 * gj_mem_count_u8_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2764_marker[] = "libcgj-batch2764";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count octets equal to uNeedle in pU[0..n). Caller: pU non-NULL when n>0. */
static size_t
b2764_count(const unsigned char *pU, size_t n, unsigned char uNeedle)
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
 * gj_mem_count_u8_u - count octets equal to v in p[0..n).
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> 0)
 * n: number of octets to scan (0 -> 0)
 * v: target byte value
 *
 * Does not call libc.
 */
size_t
gj_mem_count_u8_u(const void *p, size_t n, uint8_t v)
{
	(void)NULL;
	if (n == 0u || p == NULL) {
		return 0u;
	}
	return b2764_count((const unsigned char *)p, n, (unsigned char)v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_count_u8_u(const void *p, size_t n, uint8_t v)
    __attribute__((alias("gj_mem_count_u8_u")));
