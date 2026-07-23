/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2814: hex decode length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hex_dec_len_u(uint32_t n);
 *     - Decoded octet count for a hex string of length n:
 *       n even → n / 2; n odd → 0 (not a complete hex pair stream).
 *   uint32_t __gj_hex_dec_len_u  (alias)
 *   __libcgj_batch2814_marker = "libcgj-batch2814"
 *
 * Encoding length exclusive wave (2811-2820). Companion to
 * gj_hex_enc_len_u (batch2813). Distinct from gj_hex_dec_len
 * (size_t, batch823) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2814_marker[] = "libcgj-batch2814";

/* ---- freestanding helpers ---------------------------------------------- */

/* Decoded byte count for n hex chars; odd n → 0. */
static uint32_t
b2814_dec_len(uint32_t uN)
{
	if ((uN & 1u) != 0u) {
		return 0u;
	}
	return uN / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_dec_len_u - decoded byte count for n hex characters.
 *
 * Odd n cannot map to whole octets → 0. Even n → n / 2 (including 0).
 */
uint32_t
gj_hex_dec_len_u(uint32_t uN)
{
	(void)NULL;
	return b2814_dec_len(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hex_dec_len_u(uint32_t uN)
    __attribute__((alias("gj_hex_dec_len_u")));
