/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3682: ZigZag encode for int32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zigzag_encode_i32_u(int32_t v);
 *     - Map signed int32 to unsigned so small |v| stay small on the wire:
 *       (v << 1) ^ (v >> 31) with arithmetic right-shift of the sign bit.
 *       Non-negative n → 2n; negative n → 2·|n|−1 (so −1 → 1, −2 → 3, …).
 *   uint32_t __gj_zigzag_encode_i32_u  (alias)
 *   __libcgj_batch3682_marker = "libcgj-batch3682"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_zigzag_encode_i32 (batch2567) and gj_zigzag_encode (batch166) —
 * unique _u surface; no multi-def. Inverse is gj_zigzag_decode_u32_u
 * (batch3683). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3682_marker[] = "libcgj-batch3682";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ZigZag-encode signed i32V as unsigned.
 * Right-shift of i32V must be arithmetic (sign-extending).
 */
static uint32_t
b3682_zz_enc(int32_t i32V)
{
	return ((uint32_t)i32V << 1) ^ (uint32_t)(i32V >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_encode_i32_u — encode signed v as an unsigned ZigZag value.
 *
 * Examples: 0 → 0, −1 → 1, 1 → 2, −2 → 3, 2 → 4.
 */
uint32_t
gj_zigzag_encode_i32_u(int32_t v)
{
	(void)NULL;
	return b3682_zz_enc(v);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zigzag_encode_i32_u(int32_t v)
    __attribute__((alias("gj_zigzag_encode_i32_u")));
