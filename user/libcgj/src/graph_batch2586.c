/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2586: base64 decode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base64_decode_len_u(size_t n);
 *     - Maximum decoded octets for n base64 characters (RFC 4648 bit
 *       capacity): floor(n * 3 / 4). n == 0 → 0. Overflow-safe form:
 *       (n / 4) * 3 + ((n % 4) * 3) / 4.
 *   size_t __gj_base64_decode_len_u  (alias)
 *   __libcgj_batch2586_marker = "libcgj-batch2586"
 *
 * Companion to gj_base64_encode_len_u (batch2585). Distinct from
 * gj_base64_dec_max (batch694) and base64_decode (batch40).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2586_marker[] = "libcgj-batch2586";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor(n * 3 / 4) without intermediate n * 3 overflow:
 *   (n / 4) * 3 + ((n % 4) * 3) / 4
 */
static size_t
b2586_dec_bytes(size_t cEnc)
{
	return (cEnc / 4u) * 3u + ((cEnc % 4u) * 3u) / 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_decode_len_u — max decoded octets for n base64 characters.
 *
 * n: encoded character count (alphabet or padding counted as length).
 * Does not inspect content. Upper bound for buffer sizing.
 */
size_t
gj_base64_decode_len_u(size_t cEnc)
{
	(void)NULL;
	return b2586_dec_bytes(cEnc);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_decode_len_u(size_t cEnc)
    __attribute__((alias("gj_base64_decode_len_u")));
