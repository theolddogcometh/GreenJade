/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2569: protobuf-style unsigned varint length.
 *
 * Surface (unique symbols):
 *   unsigned gj_varint_u64_len(uint64_t v);
 *     — Byte length of the canonical protobuf/LEB128 unsigned varint
 *       encoding of v (7 data bits per byte, MSB continuation).
 *       Zero encodes as one byte. Result is always in 1..10 for any
 *       uint64_t. Does not write an encoding — length probe only.
 *   unsigned __gj_varint_u64_len  (alias)
 *   __libcgj_batch2569_marker = "libcgj-batch2569"
 *
 * Compression exclusive wave (2561–2570). Complements gj_varint_encode
 * (batch283) without multi-def. Distinct from gj_uleb128_* /
 * gj_pb_encode_varint / gj_quic_varint_* length helpers by public name.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2569_marker[] = "libcgj-batch2569";

/* Max encoded length of a 64-bit unsigned protobuf varint: ceil(64/7)=10. */
#define B2569_VARINT_MAX 10u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical protobuf unsigned varint length of u64V.
 * Each iteration peels 7 payload bits; stops when no more bits remain.
 */
static unsigned
b2569_len(uint64_t u64V)
{
	unsigned uN;

	uN = 0u;
	do {
		u64V >>= 7;
		uN++;
	} while (u64V != 0u && uN < (unsigned)B2569_VARINT_MAX);

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_varint_u64_len — encoded byte count for protobuf unsigned varint v.
 *
 * Examples: 0 → 1, 127 → 1, 128 → 2, UINT64_MAX → 10.
 */
unsigned
gj_varint_u64_len(uint64_t v)
{
	(void)NULL;
	return b2569_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_varint_u64_len(uint64_t v)
    __attribute__((alias("gj_varint_u64_len")));
