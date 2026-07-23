/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2812: max base64 decode length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_base64_dec_len_u(uint32_t n);
 *     - Return the maximum number of decoded octets that n base64
 *       encoded characters can produce (RFC 4648 bit capacity):
 *         (n * 3) / 4
 *       n == 0 → 0. Pure length math; does not inspect padding or
 *       read/write buffers. Upper bound for buffer sizing.
 *   uint32_t __gj_base64_dec_len_u  (alias)
 *   __libcgj_batch2812_marker = "libcgj-batch2812"
 *
 * Encoding length exclusive wave (2811-2820). Companion to
 * gj_base64_enc_len_u (batch2811). Distinct from gj_base64_dec_max
 * (size_t, batch694) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2812_marker[] = "libcgj-batch2812";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Max decoded octets for uN base64 characters:
 *   (n / 4) * 3 + ((n % 4) * 3) / 4
 * equals (n * 3) / 4 without intermediate n*3 overflow.
 */
static uint32_t
b2812_dec_len(uint32_t uN)
{
	return (uN / 4u) * 3u + ((uN % 4u) * 3u) / 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_dec_len_u - maximum decoded octets for n base64 characters.
 *
 * n: number of encoded characters (alphabet or padding counted as
 *    input length; this helper does not inspect content)
 *
 * Each encoded character carries 6 bits; decoded octets are 8 bits.
 * Maximum payload octets is therefore floor(n * 6 / 8) = floor(n * 3 / 4).
 *
 * Examples:
 *   n=0 → 0
 *   n=2 → 1
 *   n=3 → 2
 *   n=4 → 3
 *   n=8 → 6
 */
uint32_t
gj_base64_dec_len_u(uint32_t uN)
{
	(void)NULL;
	return b2812_dec_len(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_base64_dec_len_u(uint32_t uN)
    __attribute__((alias("gj_base64_dec_len_u")));
