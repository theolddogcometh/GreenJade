/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3686: unsigned LEB128 length for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_leb128_u32_len_u(uint32_t v);
 *     - Byte length of the canonical unsigned LEB128 / protobuf varint
 *       encoding of a 32-bit value (7 data bits per byte). Zero encodes
 *       as one byte. Result is always in 1..5 for any uint32_t.
 *       Length probe only — does not write an encoding.
 *   unsigned __gj_leb128_u32_len_u  (alias)
 *   __libcgj_batch3686_marker = "libcgj-batch3686"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_varint_u64_len / gj_varint_u64_len_u / gj_uleb128_* encode/decode —
 * unique u32 length surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3686_marker[] = "libcgj-batch3686";

/* Max encoded length of a 32-bit unsigned LEB128: ceil(32/7)=5. */
#define B3686_LEB_MAX 5u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical unsigned LEB128 length of u32V.
 * Each iteration peels 7 payload bits; stops when no more bits remain.
 */
static unsigned
b3686_len(uint32_t u32V)
{
	unsigned uN;

	uN = 0u;
	do {
		u32V >>= 7;
		uN++;
	} while (u32V != 0u && uN < (unsigned)B3686_LEB_MAX);

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_leb128_u32_len_u — encoded byte count for unsigned LEB128 of v.
 *
 * Examples: 0 → 1, 127 → 1, 128 → 2, UINT32_MAX → 5.
 */
unsigned
gj_leb128_u32_len_u(uint32_t v)
{
	(void)NULL;
	return b3686_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_leb128_u32_len_u(uint32_t v)
    __attribute__((alias("gj_leb128_u32_len_u")));
