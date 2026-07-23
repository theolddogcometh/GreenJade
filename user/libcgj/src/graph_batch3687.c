/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3687: unsigned LEB128 length for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_leb128_u64_len_u(uint64_t v);
 *     - Byte length of the canonical unsigned LEB128 / protobuf varint
 *       encoding of a 64-bit value (7 data bits per byte). Zero encodes
 *       as one byte. Result is always in 1..10 for any uint64_t.
 *       Length probe only — does not write an encoding.
 *   unsigned __gj_leb128_u64_len_u  (alias)
 *   __libcgj_batch3687_marker = "libcgj-batch3687"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_varint_u64_len (batch2569) / gj_varint_u64_len_u (batch3681) by
 * public name (leb128 vs varint) — unique surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3687_marker[] = "libcgj-batch3687";

/* Max encoded length of a 64-bit unsigned LEB128: ceil(64/7)=10. */
#define B3687_LEB_MAX 10u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical unsigned LEB128 length of u64V.
 * Each iteration peels 7 payload bits; stops when no more bits remain.
 */
static unsigned
b3687_len(uint64_t u64V)
{
	unsigned uN;

	uN = 0u;
	do {
		u64V >>= 7;
		uN++;
	} while (u64V != 0u && uN < (unsigned)B3687_LEB_MAX);

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_leb128_u64_len_u — encoded byte count for unsigned LEB128 of v.
 *
 * Examples: 0 → 1, 127 → 1, 128 → 2, UINT64_MAX → 10.
 */
unsigned
gj_leb128_u64_len_u(uint64_t v)
{
	(void)NULL;
	return b3687_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_leb128_u64_len_u(uint64_t v)
    __attribute__((alias("gj_leb128_u64_len_u")));
