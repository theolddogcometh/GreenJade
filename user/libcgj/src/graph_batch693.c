/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch693: padded base64 encode length.
 *
 * Surface (unique symbols):
 *   size_t gj_base64_enc_len(size_t n);
 *     — Return the number of base64 output characters needed to encode
 *       n input octets with standard '=' padding:
 *         4 * ((n + 2) / 3)
 *       n == 0 → 0. Pure length math; does not read or write buffers.
 *   size_t __gj_base64_enc_len  (alias)
 *   __libcgj_batch693_marker = "libcgj-batch693"
 *
 * Distinct from encode/decode surfaces (base64_encode in graph_batch40,
 * gj_base64url_enc in graph_batch551, etc.). Unique gj_ names only;
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch693_marker[] = "libcgj-batch693";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_enc_len — padded base64 encode output length for n octets.
 *
 * n: number of input octets
 *
 * Each group of up to 3 input octets becomes 4 output characters
 * (with '=' padding when n is not a multiple of 3). Equivalent to:
 *   4 * ceil(n / 3)  computed as  4 * ((n + 2) / 3)
 * with unsigned integer division. Returns 0 when n is 0.
 */
size_t
gj_base64_enc_len(size_t n)
{
	return 4u * ((n + 2u) / 3u);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_enc_len(size_t n)
    __attribute__((alias("gj_base64_enc_len")));
