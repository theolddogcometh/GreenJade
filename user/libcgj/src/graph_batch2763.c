/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2763: freestanding first-match index for a u8.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_find_u8_u(const void *p, size_t n, uint8_t v);
 *     - Return the least index i in p[0..n) with p[i] == v, or n if
 *       no match (also when n == 0). If n > 0 and p is NULL, returns n.
 *       Binary-safe: n is the sole bound (embedded NULs are ordinary).
 *   size_t __gj_mem_find_u8_u  (alias)
 *   __libcgj_batch2763_marker = "libcgj-batch2763"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_mem_chr / gj_mem_chr_n / gj_mem_chr_n_u (pointer forms) and
 * gj_mem_len_until_u - unique index-of-equal gj_mem_find_u8_u surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2763_marker[] = "libcgj-batch2763";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index with pU[i] == uNeedle, or cb if none. */
static size_t
b2763_find(const unsigned char *pU, size_t cb, unsigned char uNeedle)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		if (pU[i] == uNeedle) {
			return i;
		}
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_find_u8_u - index of first octet equal to v in p[0..n), or n.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> n)
 * n: number of octets to scan (0 -> 0)
 * v: target byte value
 *
 * Does not call libc. Early-exits on the first match.
 */
size_t
gj_mem_find_u8_u(const void *p, size_t n, uint8_t v)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (p == NULL) {
		return n;
	}
	return b2763_find((const unsigned char *)p, n, (unsigned char)v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_find_u8_u(const void *p, size_t n, uint8_t v)
    __attribute__((alias("gj_mem_find_u8_u")));
