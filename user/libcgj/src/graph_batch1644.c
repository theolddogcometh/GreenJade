/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1644: RFC 4648 base32 decode length hint.
 *
 * Surface (unique symbols):
 *   size_t gj_base32_decode_len(size_t n);
 *     — Maximum decoded byte count for a padded (or unpadded) base32
 *       string of n ASCII characters: (n * 5) / 8. Padding '=' is counted
 *       in n the same as data; callers that strip padding get a slightly
 *       tighter bound via the same formula. n == 0 → 0.
 *   size_t __gj_base32_decode_len  (alias)
 *   __libcgj_batch1644_marker = "libcgj-batch1644"
 *
 * Does NOT redefine base32_decode (graph_batch41) — length-only helper.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1644_marker[] = "libcgj-batch1644";

/* ---- freestanding helpers ---------------------------------------------- */

/* 5 bits per char → floor(n * 5 / 8) raw bytes upper-bound for decode out. */
static size_t
b1644_dec_bytes(size_t cEnc)
{
	return (cEnc * 5u) / 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_decode_len — max decoded bytes for an n-char base32 payload.
 */
size_t
gj_base32_decode_len(size_t cEnc)
{
	return b1644_dec_bytes(cEnc);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_decode_len(size_t cEnc)
    __attribute__((alias("gj_base32_decode_len")));
