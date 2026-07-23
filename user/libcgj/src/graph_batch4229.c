/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4229: freestanding bounded octet fill (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_fill_u(void *dst, uint8_t b, size_t n);
 *     - Fill n octets at dst with value b. Returns n on success.
 *       dst == NULL -> return 0 (no write). n == 0 -> return 0.
 *   size_t __gj_mem_fill_u  (alias)
 *   __libcgj_batch4229_marker = "libcgj-batch4229"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Unique gj_mem_fill_u surface only; no multi-def.
 * Distinct from gj_mem_fill_u8 (batch608) / gj_mem_fill_u8_n (batch2219)
 * / gj_mem_set (batch396) and libc memset. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4229_marker[] = "libcgj-batch4229";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Fill pD[0..n) with uVal. Caller: pD non-NULL, n > 0. Returns n.
 */
static size_t
b4229_fill(unsigned char *pD, unsigned char uVal, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pD[i] = uVal;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_fill_u - fill n octets at dst with byte b; return n on success.
 *
 * dst: destination of at least n bytes when n > 0 (NULL -> 0, no write)
 * b:   fill byte
 * n:   number of octets to write (0 -> 0)
 *
 * Does not call libc memset.
 * No parent wires.
 */
size_t
gj_mem_fill_u(void *dst, uint8_t b, size_t n)
{
	(void)NULL;
	if (dst == NULL || n == 0u) {
		return 0u;
	}
	return b4229_fill((unsigned char *)dst, (unsigned char)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_fill_u(void *dst, uint8_t b, size_t n)
    __attribute__((alias("gj_mem_fill_u")));
