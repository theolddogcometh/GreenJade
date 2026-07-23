/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch743: padded Base32 encode length (RFC 4648).
 *
 * Surface (unique symbols):
 *   size_t gj_base32_enc_len(size_t n);
 *     — Output character count for padded Base32 of n input bytes:
 *       8 * ((n + 4) / 5). Does not include a trailing NUL.
 *   size_t __gj_base32_enc_len  (alias)
 *   __libcgj_batch743_marker = "libcgj-batch743"
 *
 * Does NOT redefine base32_encode / base32_decode (batch41) or
 * base32hex_* (batch42) — avoid multi-def. Length helper only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch743_marker[] = "libcgj-batch743";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_enc_len — padded Base32 encoded length for n source bytes.
 *
 * Each group of up to 5 input bytes becomes 8 output alphabet/pad chars.
 * n == 0 → 0.
 */
size_t
gj_base32_enc_len(size_t n)
{
	return (size_t)8u * ((n + 4u) / 5u);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_enc_len(size_t n)
    __attribute__((alias("gj_base32_enc_len")));
