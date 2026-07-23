/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7882: C-locale strxfrm stub (identity).
 *
 * Surface (unique symbols):
 *   size_t gj_strxfrm_c_7882(char *dst, const char *src, size_t n);
 *     - C-locale transform: identity copy. Returns length of src not
 *       counting NUL. If dst != NULL and n > 0, copies min(len, n-1)
 *       octets and NUL-terminates. src NULL -> 0 (dst untouched).
 *   size_t __gj_strxfrm_c_7882  (alias)
 *   __libcgj_batch7882_marker = "libcgj-batch7882"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strxfrm_c_7882 surface only; no multi-def with libc
 * strxfrm. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7882_marker[] = "libcgj-batch7882";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of NUL-terminated p (not counting NUL). p non-NULL. */
static size_t
b7882_len(const unsigned char *p)
{
	size_t n;

	n = 0u;
	while (p[n] != 0u) {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strxfrm_c_7882 - C-locale strxfrm identity stub.
 *
 * dst: destination buffer of at least n bytes when n > 0 (may be NULL)
 * src: source NUL string (NULL -> 0)
 * n:   capacity of dst including room for NUL when writing
 *
 * Returns src length (C-locale weight length). Soft freestanding only;
 * does not call libc strxfrm. No parent wires.
 */
size_t
gj_strxfrm_c_7882(char *dst, const char *src, size_t n)
{
	const unsigned char *pSrc;
	size_t nLen;
	size_t nCopy;
	size_t i;

	(void)NULL;
	if (src == NULL) {
		return 0u;
	}

	pSrc = (const unsigned char *)src;
	nLen = b7882_len(pSrc);

	if (dst != NULL && n > 0u) {
		nCopy = nLen;
		if (nCopy > (n - 1u)) {
			nCopy = n - 1u;
		}
		for (i = 0u; i < nCopy; i++) {
			dst[i] = (char)pSrc[i];
		}
		dst[nCopy] = '\0';
	}
	return nLen;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strxfrm_c_7882(char *dst, const char *src, size_t n)
    __attribute__((alias("gj_strxfrm_c_7882")));
