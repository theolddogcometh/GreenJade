/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3681: protobuf-style unsigned varint length (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_varint_u64_len_u(uint64_t v);
 *     - Byte length of the canonical protobuf/LEB128 unsigned varint
 *       encoding of v (7 data bits per byte, MSB continuation).
 *       Zero encodes as one byte. Result is always in 1..10 for any
 *       uint64_t. Does not write an encoding — length probe only.
 *   unsigned __gj_varint_u64_len_u  (alias)
 *   __libcgj_batch3681_marker = "libcgj-batch3681"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690: varint_u64_len_u,
 * zigzag_encode_i32_u, zigzag_decode_u32_u, zigzag_encode_i64_u,
 * zigzag_decode_u64_u, leb128_u32_len_u, leb128_u64_len_u,
 * sleb128_i32_len_u, sleb128_i64_len_u, batch_id_3690). Distinct from
 * gj_varint_u64_len (batch2569) — unique _u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3681_marker[] = "libcgj-batch3681";

/* Max encoded length of a 64-bit unsigned protobuf varint: ceil(64/7)=10. */
#define B3681_VARINT_MAX 10u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical protobuf unsigned varint length of u64V.
 * Each iteration peels 7 payload bits; stops when no more bits remain.
 */
static unsigned
b3681_len(uint64_t u64V)
{
	unsigned uN;

	uN = 0u;
	do {
		u64V >>= 7;
		uN++;
	} while (u64V != 0u && uN < (unsigned)B3681_VARINT_MAX);

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_varint_u64_len_u — encoded byte count for protobuf unsigned varint v.
 *
 * Examples: 0 → 1, 127 → 1, 128 → 2, UINT64_MAX → 10.
 */
unsigned
gj_varint_u64_len_u(uint64_t v)
{
	(void)NULL;
	return b3681_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_varint_u64_len_u(uint64_t v)
    __attribute__((alias("gj_varint_u64_len_u")));
