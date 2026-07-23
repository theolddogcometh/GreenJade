/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2567: ZigZag encode for int32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zigzag_encode_i32(int32_t v);
 *     — Map signed int32 to unsigned so small |v| stay small on the wire:
 *       (v << 1) ^ (v >> 31) with arithmetic right-shift of the sign bit.
 *       Non-negative n → 2n; negative n → 2·|n|−1 (so −1 → 1, −2 → 3, …).
 *   uint32_t __gj_zigzag_encode_i32  (alias)
 *   __libcgj_batch2567_marker = "libcgj-batch2567"
 *
 * Compression exclusive wave (2561–2570). Distinct from
 * gj_zigzag_encode (batch166 int64) — unique i32 surface, no multi-def.
 * Inverse is gj_zigzag_decode_i32 (batch2568).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2567_marker[] = "libcgj-batch2567";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ZigZag-encode signed i32V as unsigned.
 * Right-shift of i32V must be arithmetic (sign-extending).
 */
static uint32_t
b2567_zz_enc(int32_t i32V)
{
	return ((uint32_t)i32V << 1) ^ (uint32_t)(i32V >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_encode_i32 — encode signed v as an unsigned ZigZag value.
 *
 * Examples: 0 → 0, −1 → 1, 1 → 2, −2 → 3, 2 → 4.
 */
uint32_t
gj_zigzag_encode_i32(int32_t v)
{
	(void)NULL;
	return b2567_zz_enc(v);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zigzag_encode_i32(int32_t v)
    __attribute__((alias("gj_zigzag_encode_i32")));
