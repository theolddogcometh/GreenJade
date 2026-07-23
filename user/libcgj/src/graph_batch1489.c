/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1489: UTF-16 surrogate pair → scalar.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf16_pair_to_cp(uint16_t hi, uint16_t lo);
 *     — Combine a high surrogate (U+D800..U+DBFF) and low surrogate
 *       (U+DC00..U+DFFF) into a Unicode scalar in U+10000..U+10FFFF.
 *       Returns 0 if hi/lo are not a valid surrogate pair.
 *       (Valid pairs never encode U+0000, so 0 is unambiguous failure.)
 *   uint32_t __gj_utf16_pair_to_cp  (alias)
 *   __libcgj_batch1489_marker = "libcgj-batch1489"
 *
 * Self-contained; does not call batch1486/1487/1488.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1489_marker[] = "libcgj-batch1489";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1489_is_high(uint16_t u)
{
	return (u >= 0xD800u && u <= 0xDBFFu) ? 1 : 0;
}

static int
b1489_is_low(uint16_t u)
{
	return (u >= 0xDC00u && u <= 0xDFFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_pair_to_cp — map (high, low) surrogate pair to a scalar.
 *
 * Formula: cp = 0x10000 + ((hi - 0xD800) << 10) + (lo - 0xDC00)
 * Range of well-formed pairs: U+10000..U+10FFFF.
 */
uint32_t
gj_utf16_pair_to_cp(uint16_t hi, uint16_t lo)
{
	uint32_t u32Hi;
	uint32_t u32Lo;
	uint32_t u32Cp;

	if (b1489_is_high(hi) == 0 || b1489_is_low(lo) == 0) {
		return 0u;
	}

	u32Hi = (uint32_t)hi - 0xD800u;
	u32Lo = (uint32_t)lo - 0xDC00u;
	u32Cp = 0x10000u + (u32Hi << 10) + u32Lo;

	/* Defensive: well-formed pairs always land in range. */
	if (u32Cp < 0x10000u || u32Cp > 0x10FFFFu) {
		return 0u;
	}
	return u32Cp;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf16_pair_to_cp(uint16_t hi, uint16_t lo)
    __attribute__((alias("gj_utf16_pair_to_cp")));
