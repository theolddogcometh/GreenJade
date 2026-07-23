/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2584: base32 decode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base32_decode_len_u(size_t n);
 *     - Maximum decoded byte count for an n-character base32 payload:
 *       (n * 5) / 8. Padding '=' counted the same as data for the bound.
 *       n == 0 → 0.
 *   size_t __gj_base32_decode_len_u  (alias)
 *   __libcgj_batch2584_marker = "libcgj-batch2584"
 *
 * Companion to gj_base32_encode_len_u (batch2583). Distinct from
 * gj_base32_dec_max (batch744) and gj_base32_decode_len (batch1644).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2584_marker[] = "libcgj-batch2584";

/* ---- freestanding helpers ---------------------------------------------- */

/* 5 bits per char → floor(n * 5 / 8) raw-byte upper bound. */
static size_t
b2584_dec_bytes(size_t cEnc)
{
	return (cEnc * 5u) / 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_decode_len_u — max decoded bytes for n base32 characters.
 *
 * n: encoded character count (alphabet or padding).
 * Actual decode may be smaller when padding is present. Pure length math.
 */
size_t
gj_base32_decode_len_u(size_t cEnc)
{
	(void)NULL;
	return b2584_dec_bytes(cEnc);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_decode_len_u(size_t cEnc)
    __attribute__((alias("gj_base32_decode_len_u")));
