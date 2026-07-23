/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch823: hex decode length (even n → n/2, else 0).
 *
 * Surface (unique symbols):
 *   size_t gj_hex_dec_len(size_t n);
 *     — Decoded octet count for a hex string of length n:
 *       n even → n / 2; n odd → 0 (not a complete hex pair stream).
 *   size_t __gj_hex_dec_len  (alias)
 *   __libcgj_batch823_marker = "libcgj-batch823"
 *
 * Companion length helper to gj_hex_enc_len (batch822). Distinct from
 * encode/decode surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch823_marker[] = "libcgj-batch823";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_dec_len — decoded byte count for n hex characters.
 *
 * Odd n cannot map to whole octets → 0. Even n → n / 2 (including 0).
 */
size_t
gj_hex_dec_len(size_t n)
{
	if ((n & 1u) != 0u) {
		return 0u;
	}
	return n / 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_dec_len(size_t n)
    __attribute__((alias("gj_hex_dec_len")));
