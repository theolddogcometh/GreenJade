/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3712: uint32 hex format buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_u32_to_hex_need_u(uint32_t v);
 *     - Bytes needed (including trailing NUL) for the minimal hex ASCII
 *       rendering of v without a "0x" prefix (no leading zeros except
 *       for v == 0 → "0"). Range is [2, 9]; UINT32_MAX needs 9
 *       (8 digits + NUL).
 *   size_t __gj_u32_to_hex_need_u  (alias)
 *   __libcgj_batch3712_marker = "libcgj-batch3712"
 *
 * Exclusive continuum CREATE-ONLY (3711-3720). Distinct from
 * gj_format_u64_hex (batch393 write), gj_u8_to_hex_pair_u (batch3375
 * fixed 2 digits) — length-only need surface; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3712_marker[] = "libcgj-batch3712";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3712_hex_digits — significant hex digit length of u; 1 when u == 0.
 * Range [1, 8] for uint32_t.
 */
static size_t
b3712_hex_digits(uint32_t u)
{
	size_t n;

	if (u == 0u) {
		return 1u;
	}
	n = 0u;
	while (u != 0u) {
		n++;
		u >>= 4;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_hex_need_u - bytes needed (incl. NUL) for hex of v.
 *
 * Minimal significant hex digits (lowercase/uppercase independent);
 * no "0x" prefix. Zero is "0" (need 2). Length-only; no write.
 *
 * Examples (return includes trailing NUL):
 *   0          → 2  ("0")
 *   255        → 3  ("ff")
 *   0xffffffff → 9  ("ffffffff")
 */
size_t
gj_u32_to_hex_need_u(uint32_t v)
{
	(void)NULL;
	return b3712_hex_digits(v) + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_to_hex_need_u(uint32_t v)
    __attribute__((alias("gj_u32_to_hex_need_u")));
