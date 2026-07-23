/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3689: signed LEB128 length for int64_t (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_sleb128_i64_len_u(int64_t v);
 *     - Byte length of the canonical signed LEB128 (DWARF/WASM-style)
 *       encoding of a 64-bit signed value. Groups 7 payload bits with
 *       arithmetic sign extension; stops when remaining bits are pure
 *       sign extension of the last group's sign bit. Result is always
 *       in 1..10 for any int64_t. Length probe only.
 *   unsigned __gj_sleb128_i64_len_u  (alias)
 *   __libcgj_batch3689_marker = "libcgj-batch3689"
 *
 * Exclusive continuum CREATE-ONLY (3681-3690). Distinct from
 * gj_sleb128_encode/decode (batch130) and gj_leb128s_* (batch284) —
 * unique i64 length surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3689_marker[] = "libcgj-batch3689";

/* Max encoded length of a 64-bit signed LEB128: ceil(64/7)=10. */
#define B3689_SLEB_MAX 10u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Canonical signed LEB128 length of i64V.
 * Arithmetic right-shift; stop when remaining is pure sign extension.
 */
static unsigned
b3689_len(int64_t i64V)
{
	unsigned uN;
	int bMore;

	uN = 0u;
	bMore = 1;
	while (bMore != 0 && uN < (unsigned)B3689_SLEB_MAX) {
		unsigned char b;

		b = (unsigned char)((uint64_t)i64V & 0x7fu);
		i64V >>= 7;
		if ((i64V == 0 && (b & 0x40u) == 0u) ||
		    (i64V == (int64_t)-1 && (b & 0x40u) != 0u)) {
			bMore = 0;
		}
		uN++;
	}

	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sleb128_i64_len_u — encoded byte count for signed LEB128 of v.
 *
 * Examples: 0 → 1, −1 → 1, 63 → 1, 64 → 2, INT64_MIN → 10.
 */
unsigned
gj_sleb128_i64_len_u(int64_t v)
{
	(void)NULL;
	return b3689_len(v);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_sleb128_i64_len_u(int64_t v)
    __attribute__((alias("gj_sleb128_i64_len_u")));
