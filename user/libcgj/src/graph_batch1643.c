/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1643: RFC 4648 base32 padded encode length.
 *
 * Surface (unique symbols):
 *   size_t gj_base32_encode_len(size_t n);
 *     — Number of ASCII characters produced by a padded RFC 4648 base32
 *       encode of n input bytes (excludes trailing NUL). Always a multiple
 *       of 8: ((n + 4) / 5) * 8. n == 0 → 0.
 *   size_t __gj_base32_encode_len  (alias)
 *   __libcgj_batch1643_marker = "libcgj-batch1643"
 *
 * Does NOT redefine base32_encode (graph_batch41) — length-only helper.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1643_marker[] = "libcgj-batch1643";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ceiling divide n by 5 then * 8 — each 5-byte group → 8 base32 chars.
 * ((n + 4) / 5) * 8 is exact for the padded codec.
 */
static size_t
b1643_enc_chars(size_t cbIn)
{
	if (cbIn == 0u) {
		return 0u;
	}
	return ((cbIn + 4u) / 5u) * 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_encode_len — padded base32 output character count for n bytes.
 */
size_t
gj_base32_encode_len(size_t cbIn)
{
	return b1643_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_encode_len(size_t cbIn)
    __attribute__((alias("gj_base32_encode_len")));
