/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5005: test whether any byte of a uint32_t equals b.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_has_byte_u(uint32_t x, uint32_t b);
 *     - Return 1 if any of the 4 bytes of x equals (b & 0xFF), else 0.
 *   uint32_t __gj_u32_has_byte_u  (alias)
 *   __libcgj_batch5005_marker = "libcgj-batch5005"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Distinct from
 * gj_u32_has_zero_byte_u (batch5003) — unique gj_u32_has_byte_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5005_marker[] = "libcgj-batch5005";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5005_has_byte(uint32_t u32X, uint32_t u32B)
{
	uint32_t u32Needle;
	uint32_t u32I;

	u32Needle = u32B & 0xFFu;
	for (u32I = 0u; u32I < 4u; u32I++) {
		if (((u32X >> (u32I * 8u)) & 0xFFu) == u32Needle) {
			return 1u;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_has_byte_u - 1 if any byte of x equals (b & 0xFF), else 0.
 *
 * x: 32-bit word examined as four little-endian bytes (byte 0 = LSB)
 * b: target byte (only bits 7:0 used)
 *
 * Returns 1 when any of the four bytes matches, else 0. Example:
 * gj_u32_has_byte_u(0x00112233u, 0x22u) -> 1.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_has_byte_u(uint32_t u32X, uint32_t u32B)
{
	(void)NULL;
	return b5005_has_byte(u32X, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_has_byte_u(uint32_t u32X, uint32_t u32B)
    __attribute__((alias("gj_u32_has_byte_u")));
