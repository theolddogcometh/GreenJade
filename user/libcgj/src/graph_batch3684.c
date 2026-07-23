/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3684: ZigZag encode for int64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_zigzag_encode_i64_u(int64_t v);
 *     - Map signed int64 to unsigned so small |v| stay small on the wire:
 *       (v << 1) ^ (v >> 63) with arithmetic right-shift of the sign bit.
 *       Non-negative n → 2n; negative n → 2·|n|−1 (so −1 → 1, −2 → 3, …).
 *   uint64_t __gj_zigzag_encode_i64_u  (alias)
 *   __libcgj_batch3684_marker = "libcgj-batch3684"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_zigzag_encode (batch166) — unique i64_u surface; no multi-def.
 * Inverse is gj_zigzag_decode_u64_u (batch3685). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3684_marker[] = "libcgj-batch3684";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ZigZag-encode signed i64V as unsigned.
 * Right-shift of i64V must be arithmetic (sign-extending).
 */
static uint64_t
b3684_zz_enc(int64_t i64V)
{
	return ((uint64_t)i64V << 1) ^ (uint64_t)(i64V >> 63);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_encode_i64_u — encode signed v as an unsigned ZigZag value.
 *
 * Examples: 0 → 0, −1 → 1, 1 → 2, −2 → 3, 2 → 4.
 */
uint64_t
gj_zigzag_encode_i64_u(int64_t v)
{
	(void)NULL;
	return b3684_zz_enc(v);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_zigzag_encode_i64_u(int64_t v)
    __attribute__((alias("gj_zigzag_encode_i64_u")));
