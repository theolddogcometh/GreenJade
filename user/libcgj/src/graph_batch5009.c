/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5009: sum of the four bytes of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bytes_sum_u(uint32_t x);
 *     - Return the arithmetic sum of the four bytes of x (range 0..1020).
 *   uint32_t __gj_u32_bytes_sum_u  (alias)
 *   __libcgj_batch5009_marker = "libcgj-batch5009"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Unique
 * gj_u32_bytes_sum_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5009_marker[] = "libcgj-batch5009";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5009_bytes_sum(uint32_t u32X)
{
	uint32_t u32Sum;
	uint32_t u32I;

	u32Sum = 0u;
	for (u32I = 0u; u32I < 4u; u32I++) {
		u32Sum += (u32X >> (u32I * 8u)) & 0xFFu;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bytes_sum_u - sum the four bytes of a 32-bit word.
 *
 * x: 32-bit word examined as four little-endian bytes (byte 0 = LSB)
 *
 * Returns the sum of all four bytes (0 when x is 0, up to 4*255 = 1020).
 * Example: gj_u32_bytes_sum_u(0x01020304u) -> 10.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bytes_sum_u(uint32_t u32X)
{
	(void)NULL;
	return b5009_bytes_sum(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bytes_sum_u(uint32_t u32X)
    __attribute__((alias("gj_u32_bytes_sum_u")));
