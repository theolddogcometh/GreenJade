/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7888: C-locale strxfrm-into with fit flag.
 *
 * Surface (unique symbols):
 *   size_t gj_strxfrm_into_7888(char *dst, size_t dst_n,
 *                               const char *src, int *fit_out);
 *     - Identity C-locale transform of src into dst. Returns weight
 *       length (src len). If fit_out != NULL, writes 1 when dst held the
 *       full transform plus NUL (dst_n > len), else 0. src NULL -> 0
 *       and fit_out 1 when present. dst may be NULL (probe only).
 *   size_t __gj_strxfrm_into_7888  (alias)
 *   __libcgj_batch7888_marker = "libcgj-batch7888"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strxfrm_into_7888 surface only; no multi-def. Distinct from
 * gj_strxfrm_c_7882. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7888_marker[] = "libcgj-batch7888";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b7888_len(const unsigned char *p)
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
 * gj_strxfrm_into_7888 - C-locale strxfrm with explicit fit out-param.
 *
 * dst:    destination (may be NULL to probe)
 * dst_n:  capacity of dst when non-NULL
 * src:    source NUL string
 * fit_out: optional; 1 if full transform + NUL fit
 *
 * Soft freestanding only. No parent wires.
 */
size_t
gj_strxfrm_into_7888(char *dst, size_t dst_n, const char *src, int *fit_out)
{
	const unsigned char *pSrc;
	size_t nLen;
	size_t nCopy;
	size_t i;
	int nFit;

	(void)NULL;
	if (src == NULL) {
		if (fit_out != NULL) {
			*fit_out = 1;
		}
		return 0u;
	}

	pSrc = (const unsigned char *)src;
	nLen = b7888_len(pSrc);
	nFit = (dst_n > nLen) ? 1 : 0;

	if (dst != NULL && dst_n > 0u) {
		nCopy = nLen;
		if (nCopy > (dst_n - 1u)) {
			nCopy = dst_n - 1u;
		}
		for (i = 0u; i < nCopy; i++) {
			dst[i] = (char)pSrc[i];
		}
		dst[nCopy] = '\0';
	}

	if (fit_out != NULL) {
		*fit_out = nFit;
	}
	return nLen;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strxfrm_into_7888(char *dst, size_t dst_n, const char *src,
    int *fit_out)
    __attribute__((alias("gj_strxfrm_into_7888")));
