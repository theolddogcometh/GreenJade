/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4556: hex encode length (size_t, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_hex_encode_len_u(size_t n);
 *     - Output character count for hex-encoding n octets: 2 * n.
 *       Does not include a trailing NUL.
 *   size_t __gj_hex_encode_len_u  (alias)
 *   __libcgj_batch4556_marker = "libcgj-batch4556"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from gj_hex_enc_len
 * (batch822) and gj_hex_enc_len_u (batch2813, uint32_t) — unique
 * gj_hex_encode_len_u surface (size_t, 2*n); no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4556_marker[] = "libcgj-batch4556";

/* ---- freestanding helpers ---------------------------------------------- */

/* Hex encode length: 2 * n. */
static size_t
b4556_enc_len(size_t n)
{
	return n * 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_encode_len_u - hex encode output length for n source bytes.
 *
 * Each input octet becomes two hex digits. n == 0 → 0. Returns 2 * n.
 * Self-contained; no parent wires.
 */
size_t
gj_hex_encode_len_u(size_t n)
{
	(void)NULL;
	return b4556_enc_len(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_encode_len_u(size_t n)
    __attribute__((alias("gj_hex_encode_len_u")));
