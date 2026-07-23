/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2583: base32 encode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base32_encode_len_u(size_t n);
 *     - Padded RFC 4648 base32 output character count for n input bytes:
 *       8 * ((n + 4) / 5). Excludes trailing NUL. n == 0 → 0.
 *   size_t __gj_base32_encode_len_u  (alias)
 *   __libcgj_batch2583_marker = "libcgj-batch2583"
 *
 * Distinct from gj_base32_enc_len (batch743), gj_base32_encode_len
 * (batch1643), and base32_encode (batch41) — exclusive _u surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2583_marker[] = "libcgj-batch2583";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Each group of up to 5 input bytes becomes 8 alphabet/pad chars.
 * Special-case n == 0 so ((0+4)/5)*8 does not yield a false 8.
 */
static size_t
b2583_enc_chars(size_t cbIn)
{
	if (cbIn == 0u) {
		return 0u;
	}
	return ((cbIn + 4u) / 5u) * 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_encode_len_u — padded base32 character count for n octets.
 *
 * n: number of source octets.
 * Returns a multiple of 8 (or 0). Pure length math; no buffer I/O.
 */
size_t
gj_base32_encode_len_u(size_t cbIn)
{
	(void)NULL;
	return b2583_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_encode_len_u(size_t cbIn)
    __attribute__((alias("gj_base32_encode_len_u")));
