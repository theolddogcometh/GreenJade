/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2568: ZigZag decode for int32_t.
 * Counterpart to batch2567 gj_zigzag_encode_i32.
 *
 * Surface (unique symbols):
 *   int32_t gj_zigzag_decode_i32(uint32_t v);
 *     — Inverse of gj_zigzag_encode_i32: (v >> 1) ^ -(v & 1).
 *       Even codes → non-negative; odd codes → negative.
 *   int32_t __gj_zigzag_decode_i32  (alias)
 *   __libcgj_batch2568_marker = "libcgj-batch2568"
 *
 * Compression exclusive wave (2561–2570). Distinct from
 * gj_zigzag_decode (batch166 int64) — unique i32 surface, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2568_marker[] = "libcgj-batch2568";

/* ---- freestanding helpers ---------------------------------------------- */

/* Decode ZigZag unsigned u32V back to signed int32. */
static int32_t
b2568_zz_dec(uint32_t u32V)
{
	return (int32_t)((u32V >> 1) ^ (uint32_t)-(int32_t)(u32V & 1u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_decode_i32 — decode ZigZag unsigned v back to signed int32.
 *
 * Examples: 0 → 0, 1 → −1, 2 → 1, 3 → −2, 4 → 2.
 */
int32_t
gj_zigzag_decode_i32(uint32_t v)
{
	(void)NULL;
	return b2568_zz_dec(v);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_zigzag_decode_i32(uint32_t v)
    __attribute__((alias("gj_zigzag_decode_i32")));
