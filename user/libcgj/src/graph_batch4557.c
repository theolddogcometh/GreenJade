/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4557: hex decode length (size_t, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_hex_decode_len_u(size_t n);
 *     - Decoded octet count for a hex string of length n: n / 2
 *       (integer division; odd n truncates).
 *   size_t __gj_hex_decode_len_u  (alias)
 *   __libcgj_batch4557_marker = "libcgj-batch4557"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from gj_hex_dec_len
 * (batch823) and gj_hex_dec_len_u (batch2814, odd → 0) — unique
 * gj_hex_decode_len_u surface (size_t, n/2); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4557_marker[] = "libcgj-batch4557";

/* ---- freestanding helpers ---------------------------------------------- */

/* Decoded byte count for n hex chars: n / 2. */
static size_t
b4557_dec_len(size_t n)
{
	return n / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_decode_len_u - decoded byte count for n hex characters.
 *
 * Returns n / 2 (truncating). n == 0 or n == 1 → 0.
 * Self-contained; no parent wires.
 */
size_t
gj_hex_decode_len_u(size_t n)
{
	(void)NULL;
	return b4557_dec_len(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_decode_len_u(size_t n)
    __attribute__((alias("gj_hex_decode_len_u")));
