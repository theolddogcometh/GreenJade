/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5003: test whether any byte of a uint32_t is zero.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_has_zero_byte_u(uint32_t x);
 *     - Return 1 if any of the 4 bytes of x is 0x00, else 0.
 *   uint32_t __gj_u32_has_zero_byte_u  (alias)
 *   __libcgj_batch5003_marker = "libcgj-batch5003"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Unique
 * gj_u32_has_zero_byte_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5003_marker[] = "libcgj-batch5003";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5003_has_zero_byte(uint32_t u32X)
{
	uint32_t u32I;

	for (u32I = 0u; u32I < 4u; u32I++) {
		if (((u32X >> (u32I * 8u)) & 0xFFu) == 0u) {
			return 1u;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_has_zero_byte_u - 1 if any byte of x is zero, else 0.
 *
 * x: 32-bit word examined as four little-endian bytes (byte 0 = LSB)
 *
 * Returns 1 when any of the four bytes is 0x00, else 0. Example:
 * gj_u32_has_zero_byte_u(0x00112233u) -> 1; gj_u32_has_zero_byte_u(0xFFu) -> 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_has_zero_byte_u(uint32_t u32X)
{
	(void)NULL;
	return b5003_has_zero_byte(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_has_zero_byte_u(uint32_t u32X)
    __attribute__((alias("gj_u32_has_zero_byte_u")));
