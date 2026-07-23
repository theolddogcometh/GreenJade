/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch694: max base64 decode output length.
 *
 * Surface (unique symbols):
 *   size_t gj_base64_dec_max(size_t n);
 *     — Return the maximum number of decoded octets that n base64
 *       encoded characters can produce (RFC 4648 bit capacity):
 *         (n * 3) / 4
 *       n == 0 → 0. Pure length math; does not inspect padding or
 *       read/write buffers. Upper bound for buffer sizing.
 *   size_t __gj_base64_dec_max  (alias)
 *   __libcgj_batch694_marker = "libcgj-batch694"
 *
 * Companion to gj_base64_enc_len (batch693). Distinct from encode/decode
 * surfaces (base64_decode in graph_batch40, gj_base64url_dec in
 * graph_batch552, etc.). Unique gj_ names only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch694_marker[] = "libcgj-batch694";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_dec_max — maximum decoded octets for n base64 characters.
 *
 * n: number of encoded characters (alphabet or padding counted as
 *    input length; this helper does not inspect content)
 *
 * Each encoded character carries 6 bits; decoded octets are 8 bits.
 * Maximum payload octets is therefore floor(n * 6 / 8) = floor(n * 3 / 4).
 *
 * Computed overflow-safe as:
 *   (n / 4) * 3 + ((n % 4) * 3) / 4
 * which equals (n * 3) / 4 for all size_t n without intermediate
 * overflow of n * 3. Returns 0 when n is 0.
 *
 * Examples:
 *   n=0 → 0
 *   n=2 → 1
 *   n=3 → 2
 *   n=4 → 3
 *   n=8 → 6
 */
size_t
gj_base64_dec_max(size_t n)
{
	return (n / 4u) * 3u + ((n % 4u) * 3u) / 4u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_dec_max(size_t n)
    __attribute__((alias("gj_base64_dec_max")));
