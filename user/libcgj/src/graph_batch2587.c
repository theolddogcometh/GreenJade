/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2587: base64url encode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base64_url_encode_len_u(size_t n);
 *     - Unpadded base64url output character count for n octets:
 *         4 * (n / 3) + { 0 if n%3==0; 2 if n%3==1; 3 if n%3==2 }
 *       Excludes trailing NUL and '=' padding. n == 0 → 0.
 *   size_t __gj_base64_url_encode_len_u  (alias)
 *   __libcgj_batch2587_marker = "libcgj-batch2587"
 *
 * Distinct from gj_base64_encode_len_u (batch2585 padded),
 * gj_base64url_enc (batch551), and base64url_encode (batch93).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2587_marker[] = "libcgj-batch2587";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Unpadded base64url length matching gj_base64url_enc (batch551):
 * full triplets * 4 + remainder tail (0 / 2 / 3).
 */
static size_t
b2587_enc_chars(size_t cbIn)
{
	size_t cFull;
	size_t cRem;
	size_t cb;

	cFull = cbIn / 3u;
	cRem = cbIn % 3u;
	cb = cFull * 4u;
	if (cRem == 1u) {
		cb += 2u;
	} else if (cRem == 2u) {
		cb += 3u;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_url_encode_len_u — unpadded base64url char count for n octets.
 *
 * n: number of source octets.
 * No '=' padding in the length. Pure length math; no buffer I/O.
 */
size_t
gj_base64_url_encode_len_u(size_t cbIn)
{
	(void)NULL;
	return b2587_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_url_encode_len_u(size_t cbIn)
    __attribute__((alias("gj_base64_url_encode_len_u")));
