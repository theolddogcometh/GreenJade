/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch822: hex encode length (2 hex chars per byte).
 *
 * Surface (unique symbols):
 *   size_t gj_hex_enc_len(size_t n);
 *     — Output character count for hex-encoding n octets: 2 * n.
 *       Does not include a trailing NUL.
 *   size_t __gj_hex_enc_len  (alias)
 *   __libcgj_batch822_marker = "libcgj-batch822"
 *
 * Distinct from gj_hex_encode / gj_hex_decode (batch298) and hex_encode
 * (batch41) — length helper only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch822_marker[] = "libcgj-batch822";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_enc_len — hex encode output length for n source bytes.
 *
 * Each input octet becomes two hex digits. n == 0 → 0.
 */
size_t
gj_hex_enc_len(size_t n)
{
	return n * 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_enc_len(size_t n)
    __attribute__((alias("gj_hex_enc_len")));
