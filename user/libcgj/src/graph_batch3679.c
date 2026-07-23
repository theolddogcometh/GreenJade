/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3679: freestanding protobuf unsigned varint
 * length for uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_varint_u32_len_u(uint32_t v);
 *     - Byte length of the canonical protobuf/LEB128 unsigned varint
 *       encoding of v (7 data bits per byte, MSB continuation).
 *       Zero encodes as one byte. Result is always in 1..5 for any
 *       uint32_t. Does not write an encoding — length probe only.
 *   unsigned __gj_varint_u32_len_u  (alias)
 *   __libcgj_batch3679_marker = "libcgj-batch3679"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_varint_u32_len_u surface only; no multi-def. Distinct from
 * gj_varint_u64_len (batch2569) / gj_varint_encode (batch283) /
 * gj_pb_encode_varint (batch131). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3679_marker[] = "libcgj-batch3679";

/* Max encoded length of a 32-bit unsigned protobuf varint: ceil(32/7)=5. */
#define B3679_VARINT_MAX 5u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical protobuf unsigned varint length of u32V.
 * Each iteration peels 7 payload bits; stops when no more bits remain.
 */
static unsigned
b3679_len(uint32_t u32V)
{
	unsigned uN;

	uN = 0u;
	do {
		u32V >>= 7;
		uN++;
	} while (u32V != 0u && uN < (unsigned)B3679_VARINT_MAX);

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_varint_u32_len_u - encoded byte count for protobuf unsigned varint v.
 *
 * Examples: 0 → 1, 127 → 1, 128 → 2, UINT32_MAX → 5.
 *
 * Pure length probe. No parent wires.
 */
unsigned
gj_varint_u32_len_u(uint32_t v)
{
	(void)NULL;
	return b3679_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_varint_u32_len_u(uint32_t v)
    __attribute__((alias("gj_varint_u32_len_u")));
