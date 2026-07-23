/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch275: minimal HTML entity escape.
 *
 * Surface:
 *   int gj_html_esc(const char *in, char *out, size_t out_cap);
 *     Escape text for safe HTML text/attribute contexts:
 *       & → &amp;   < → &lt;   > → &gt;   " → &quot;   ' → &#39;
 *     All other octets pass through unchanged. out is always NUL-terminated
 *     on success. out_cap includes room for the trailing NUL.
 *     Returns 0 on success, -1 on error (NULL args or insufficient out_cap).
 *   __gj_html_esc  (alias)
 *   __libcgj_batch275_marker = "libcgj-batch275"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch275_marker[] = "libcgj-batch275";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b275_emit — append cb bytes from pSrc into szOut at *pIOut if room
 * remains for those bytes plus a final NUL (*pIOut + cb < cbCap).
 * Advances *pIOut. Returns 0 on success, -1 if out of space.
 */
static int
b275_emit(char *szOut, size_t cbCap, size_t *pIOut, const char *pSrc,
          size_t cb)
{
	size_t iByte;
	size_t iOut = *pIOut;

	if (cb > 0u && pSrc == NULL) {
		return -1;
	}
	if (iOut + cb >= cbCap) {
		return -1;
	}
	for (iByte = 0u; iByte < cb; iByte++) {
		szOut[iOut + iByte] = pSrc[iByte];
	}
	*pIOut = iOut + cb;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_html_esc — minimal HTML escape of &, <, >, ", '.
 *
 * in:      NUL-terminated input (must be non-NULL)
 * out:     destination buffer (must be non-NULL)
 * out_cap: capacity of out including trailing NUL
 *
 * Returns 0 on success, -1 on NULL args, zero capacity, or overflow.
 * On success out is always NUL-terminated. On failure out may be partial.
 */
int
gj_html_esc(const char *in, char *out, size_t out_cap)
{
	size_t iIn;
	size_t iOut;
	unsigned char uch;
	const char *pRep;
	size_t cbRep;

	if (in == NULL || out == NULL) {
		return -1;
	}
	if (out_cap == 0u) {
		return -1;
	}

	iOut = 0u;
	for (iIn = 0u; in[iIn] != '\0'; iIn++) {
		uch = (unsigned char)in[iIn];
		switch (uch) {
		case (unsigned char)'&':
			pRep = "&amp;";
			cbRep = 5u;
			break;
		case (unsigned char)'<':
			pRep = "&lt;";
			cbRep = 4u;
			break;
		case (unsigned char)'>':
			pRep = "&gt;";
			cbRep = 4u;
			break;
		case (unsigned char)'"':
			pRep = "&quot;";
			cbRep = 6u;
			break;
		case (unsigned char)'\'':
			pRep = "&#39;";
			cbRep = 5u;
			break;
		default:
			pRep = (const char *)&in[iIn];
			cbRep = 1u;
			break;
		}
		if (b275_emit(out, out_cap, &iOut, pRep, cbRep) != 0) {
			return -1;
		}
	}
	out[iOut] = '\0';
	return 0;
}

int __gj_html_esc(const char *in, char *out, size_t out_cap)
    __attribute__((alias("gj_html_esc")));
