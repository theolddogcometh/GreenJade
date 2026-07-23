/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch821: padded base64 encode length (explicit name).
 *
 * Surface (unique symbols):
 *   size_t gj_base64_enc_pad_len(size_t n);
 *     — Same padded length as gj_base64_enc_len (batch693):
 *         4 * ((n + 2) / 3)
 *       n == 0 → 0. Pure length math; no buffer I/O.
 *   size_t __gj_base64_enc_pad_len  (alias)
 *   __libcgj_batch821_marker = "libcgj-batch821"
 *
 * Does NOT redefine gj_base64_enc_len / __gj_base64_enc_len (batch693)
 * — avoid multi-def. Explicit pad-named twin only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch821_marker[] = "libcgj-batch821";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_enc_pad_len — padded base64 output character count for n octets.
 *
 * Equivalent to 4 * ceil(n / 3) via 4 * ((n + 2) / 3). n == 0 → 0.
 */
size_t
gj_base64_enc_pad_len(size_t n)
{
	return 4u * ((n + 2u) / 3u);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base64_enc_pad_len(size_t n)
    __attribute__((alias("gj_base64_enc_pad_len")));
