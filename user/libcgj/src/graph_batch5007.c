/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5007: first byte index of x equal to b.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_first_byte_eq_u(uint32_t x, uint32_t b);
 *     - Return the lowest byte index i in 0..3 where byte i of x equals
 *       (b & 0xFF). Byte 0 is the LSB. Return 4 if no match.
 *   uint32_t __gj_u32_first_byte_eq_u  (alias)
 *   __libcgj_batch5007_marker = "libcgj-batch5007"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Distinct from
 * gj_u32_has_byte_u / gj_u32_count_bytes_eq_u — unique
 * gj_u32_first_byte_eq_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5007_marker[] = "libcgj-batch5007";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5007_first_byte_eq(uint32_t u32X, uint32_t u32B)
{
	uint32_t u32Needle;
	uint32_t u32I;

	u32Needle = u32B & 0xFFu;
	for (u32I = 0u; u32I < 4u; u32I++) {
		if (((u32X >> (u32I * 8u)) & 0xFFu) == u32Needle) {
			return u32I;
		}
	}
	return 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_first_byte_eq_u - lowest index of a byte of x equal to (b & 0xFF).
 *
 * x: 32-bit word examined as four little-endian bytes (byte 0 = LSB)
 * b: target byte (only bits 7:0 used)
 *
 * Returns index in 0..3 on match, or 4 when no byte equals the needle.
 * Example: gj_u32_first_byte_eq_u(0x00112233u, 0x22u) -> 2.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_first_byte_eq_u(uint32_t u32X, uint32_t u32B)
{
	(void)NULL;
	return b5007_first_byte_eq(u32X, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_first_byte_eq_u(uint32_t u32X, uint32_t u32B)
    __attribute__((alias("gj_u32_first_byte_eq_u")));
