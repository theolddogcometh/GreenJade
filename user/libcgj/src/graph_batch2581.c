/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2581: base16 encode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base16_encode_len_u(size_t n);
 *     - Output character count for base16/hex of n octets: 2 * n.
 *       Does not include a trailing NUL. n == 0 → 0.
 *   size_t __gj_base16_encode_len_u  (alias)
 *   __libcgj_batch2581_marker = "libcgj-batch2581"
 *
 * Distinct from gj_hex_enc_len (batch822), gj_base16_enc (batch626),
 * and hex_encode (batch41) — length-only exclusive _u surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2581_marker[] = "libcgj-batch2581";

/* ---- freestanding helpers ---------------------------------------------- */

/* Two hex digits per input octet. */
static size_t
b2581_enc_chars(size_t cbIn)
{
	return cbIn * 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base16_encode_len_u — base16 encode character count for n octets.
 *
 * n: number of source octets.
 * Returns 2 * n (no trailing NUL). Pure length math.
 */
size_t
gj_base16_encode_len_u(size_t cbIn)
{
	(void)NULL;
	return b2581_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base16_encode_len_u(size_t cbIn)
    __attribute__((alias("gj_base16_encode_len_u")));
