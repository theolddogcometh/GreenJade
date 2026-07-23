/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2219: freestanding u8 fill returning count.
 *
 * Surface (unique symbols):
 *   size_t gj_mem_fill_u8_n(void *p, size_t n, unsigned char v);
 *     - Fill n octets at p with value v. Returns n on success.
 *       p == NULL -> return 0 (no write). n == 0 -> return 0.
 *   size_t __gj_mem_fill_u8_n  (alias)
 *   __libcgj_batch2219_marker = "libcgj-batch2219"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from
 * gj_mem_fill_u8 (batch608) / gj_mem_set (batch396) - returns byte
 * count and uses unique gj_mem_fill_u8_n name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2219_marker[] = "libcgj-batch2219";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fill pU[0..n) with uVal. Caller: pU non-NULL, n > 0. Returns n. */
static size_t
b2219_fill_u8(unsigned char *pU, size_t n, unsigned char uVal)
{
	size_t iByte;

	for (iByte = 0u; iByte < n; iByte++) {
		pU[iByte] = uVal;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_fill_u8_n - fill n bytes at p with unsigned char v; return n.
 *
 * p: destination region of at least n bytes when n > 0 (NULL -> 0)
 * n: number of octets to write (0 -> 0)
 * v: fill byte
 *
 * Does not call libc memset. Returns the number of octets written
 * (n on success, 0 on NULL or empty request).
 */
size_t
gj_mem_fill_u8_n(void *p, size_t n, unsigned char v)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	if (n == 0u) {
		return 0u;
	}
	return b2219_fill_u8((unsigned char *)p, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_fill_u8_n(void *p, size_t n, unsigned char v)
    __attribute__((alias("gj_mem_fill_u8_n")));
