/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4228: freestanding bounded octet copy (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_copy_u(void *dst, const void *src, size_t n);
 *     - Copy n octets from src to dst. Returns n on success. No-op if
 *       dst is NULL or src is NULL (returns 0 when n > 0). n == 0 -> 0.
 *       Regions must not overlap (forward walk only).
 *   size_t __gj_mem_copy_u  (alias)
 *   __libcgj_batch4228_marker = "libcgj-batch4228"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Unique gj_mem_copy_u surface only; no multi-def.
 * Distinct from libc memcpy / gj_mem_cpy / gj_mem_copy_fwd (batch2217).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4228_marker[] = "libcgj-batch4228";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Forward copy n octets from pS to pD. Caller: both non-NULL, n > 0.
 * Returns n.
 */
static size_t
b4228_copy(unsigned char *pD, const unsigned char *pS, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pD[i] = pS[i];
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_copy_u - copy n octets from src to dst; return n on success.
 *
 * dst: destination of at least n bytes when n > 0 (NULL -> 0, no write)
 * src: source of at least n bytes when n > 0 (NULL -> 0, no write)
 * n:   number of octets to copy (0 -> 0)
 *
 * Does not call libc memcpy. Forward walk only (non-overlapping regions).
 * No parent wires.
 */
size_t
gj_mem_copy_u(void *dst, const void *src, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (dst == NULL || src == NULL) {
		return 0u;
	}
	if (dst == (void *)(uintptr_t)(const void *)src) {
		return n;
	}
	return b4228_copy((unsigned char *)dst,
	    (const unsigned char *)src, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_copy_u(void *dst, const void *src, size_t n)
    __attribute__((alias("gj_mem_copy_u")));
