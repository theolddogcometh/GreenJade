/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3683: ZigZag decode from uint32_t (_u).
 * Counterpart to batch3682 gj_zigzag_encode_i32_u.
 *
 * Surface (unique symbols):
 *   int32_t gj_zigzag_decode_u32_u(uint32_t v);
 *     - Inverse of gj_zigzag_encode_i32_u: (v >> 1) ^ -(v & 1).
 *       Even codes → non-negative; odd codes → negative.
 *   int32_t __gj_zigzag_decode_u32_u  (alias)
 *   __libcgj_batch3683_marker = "libcgj-batch3683"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_zigzag_decode_i32 (batch2568) and gj_zigzag_decode (batch166) —
 * unique _u / u32-named surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3683_marker[] = "libcgj-batch3683";

/* ---- freestanding helpers ---------------------------------------------- */

/* Decode ZigZag unsigned u32V back to signed int32. */
static int32_t
b3683_zz_dec(uint32_t u32V)
{
	return (int32_t)((u32V >> 1) ^ (uint32_t)-(int32_t)(u32V & 1u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_decode_u32_u — decode ZigZag unsigned v back to signed int32.
 *
 * Examples: 0 → 0, 1 → −1, 2 → 1, 3 → −2, 4 → 2.
 */
int32_t
gj_zigzag_decode_u32_u(uint32_t v)
{
	(void)NULL;
	return b3683_zz_dec(v);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_zigzag_decode_u32_u(uint32_t v)
    __attribute__((alias("gj_zigzag_decode_u32_u")));
