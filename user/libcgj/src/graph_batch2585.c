/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2585: base64 encode length (exclusive _u).
 *
 * Surface (unique symbols):
 *   size_t gj_base64_encode_len_u(size_t n);
 *     - Padded RFC 4648 base64 output character count for n octets:
 *       4 * ((n + 2) / 3). Excludes trailing NUL. n == 0 → 0.
 *   size_t __gj_base64_encode_len_u  (alias)
 *   __libcgj_batch2585_marker = "libcgj-batch2585"
 *
 * Distinct from gj_base64_enc_len (batch693) and base64_encode
 * (batch40) — exclusive _u surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2585_marker[] = "libcgj-batch2585";

/* ---- freestanding helpers ---------------------------------------------- */

/* Each group of up to 3 input octets becomes 4 alphabet/pad chars. */
static size_t
b2585_enc_chars(size_t cbIn)
{
	return 4u * ((cbIn + 2u) / 3u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_encode_len_u — padded base64 character count for n octets.
 *
 * n: number of source octets.
 * Equivalent to 4 * ceil(n / 3) via unsigned division. Pure length math.
 */
size_t
gj_base64_encode_len_u(size_t cbIn)
{
	(void)NULL;
	return b2585_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_encode_len_u(size_t cbIn)
    __attribute__((alias("gj_base64_encode_len_u")));
