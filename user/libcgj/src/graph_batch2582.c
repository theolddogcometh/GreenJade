/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2582: base16 decode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base16_decode_len_u(size_t n);
 *     - Decoded octet count for a base16 string of length n:
 *       n even → n / 2; n odd → 0 (incomplete nibble pair).
 *   size_t __gj_base16_decode_len_u  (alias)
 *   __libcgj_batch2582_marker = "libcgj-batch2582"
 *
 * Companion to gj_base16_encode_len_u (batch2581). Distinct from
 * gj_hex_dec_len (batch823) and gj_base16_dec (batch627).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2582_marker[] = "libcgj-batch2582";

/* ---- freestanding helpers ---------------------------------------------- */

/* Even n → n/2 whole octets; odd n cannot form complete pairs → 0. */
static size_t
b2582_dec_bytes(size_t cEnc)
{
	if ((cEnc & 1u) != 0u) {
		return 0u;
	}
	return cEnc / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base16_decode_len_u — decoded byte count for n base16 characters.
 *
 * n: encoded character count (must be even for a full pair stream).
 * Odd n → 0. Even n → n / 2. Pure length math.
 */
size_t
gj_base16_decode_len_u(size_t cEnc)
{
	(void)NULL;
	return b2582_dec_bytes(cEnc);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base16_decode_len_u(size_t cEnc)
    __attribute__((alias("gj_base16_decode_len_u")));
