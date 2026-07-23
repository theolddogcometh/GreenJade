/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch744: max Base32 decoded byte length.
 *
 * Surface (unique symbols):
 *   size_t gj_base32_dec_max(size_t n);
 *     — Upper bound on decoded byte count for an n-character Base32
 *       input (padding ignored in the bound): (n * 5) / 8.
 *   size_t __gj_base32_dec_max  (alias)
 *   __libcgj_batch744_marker = "libcgj-batch744"
 *
 * Does NOT redefine gj_base32_enc_len (batch743), base32_encode /
 * base32_decode (batch41), or base32hex_* (batch42) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch744_marker[] = "libcgj-batch744";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base32_dec_max — maximum decoded size for n Base32 characters.
 *
 * Integer floor of 5 bits per char into whole bytes: (n * 5) / 8.
 * Actual decode may be smaller when padding is present. n == 0 → 0.
 */
size_t
gj_base32_dec_max(size_t n)
{
	return (n * 5u) / 8u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base32_dec_max(size_t n)
    __attribute__((alias("gj_base32_dec_max")));
