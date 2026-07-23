/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3685: ZigZag decode from uint64_t (_u).
 * Counterpart to batch3684 gj_zigzag_encode_i64_u.
 *
 * Surface (unique symbols):
 *   int64_t gj_zigzag_decode_u64_u(uint64_t v);
 *     - Inverse of gj_zigzag_encode_i64_u: (v >> 1) ^ -(v & 1).
 *       Even codes → non-negative; odd codes → negative.
 *   int64_t __gj_zigzag_decode_u64_u  (alias)
 *   __libcgj_batch3685_marker = "libcgj-batch3685"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_zigzag_decode (batch166) — unique u64_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3685_marker[] = "libcgj-batch3685";

/* ---- freestanding helpers ---------------------------------------------- */

/* Decode ZigZag unsigned u64V back to signed int64. */
static int64_t
b3685_zz_dec(uint64_t u64V)
{
	return (int64_t)((u64V >> 1) ^ (uint64_t)-(int64_t)(u64V & 1u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zigzag_decode_u64_u — decode ZigZag unsigned v back to signed int64.
 *
 * Examples: 0 → 0, 1 → −1, 2 → 1, 3 → −2, 4 → 2.
 */
int64_t
gj_zigzag_decode_u64_u(uint64_t v)
{
	(void)NULL;
	return b3685_zz_dec(v);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_zigzag_decode_u64_u(uint64_t v)
    __attribute__((alias("gj_zigzag_decode_u64_u")));
