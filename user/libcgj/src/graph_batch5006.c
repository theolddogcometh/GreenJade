/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5006: count bytes of a uint32_t equal to b.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_count_bytes_eq_u(uint32_t x, uint32_t b);
 *     - Count how many of the 4 bytes of x equal (b & 0xFF). Result 0..4.
 *   uint32_t __gj_u32_count_bytes_eq_u  (alias)
 *   __libcgj_batch5006_marker = "libcgj-batch5006"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Distinct from
 * gj_u32_has_byte_u (batch5005) — unique gj_u32_count_bytes_eq_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5006_marker[] = "libcgj-batch5006";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5006_count_bytes_eq(uint32_t u32X, uint32_t u32B)
{
	uint32_t u32Needle;
	uint32_t u32I;
	uint32_t u32Count;

	u32Needle = u32B & 0xFFu;
	u32Count = 0u;
	for (u32I = 0u; u32I < 4u; u32I++) {
		if (((u32X >> (u32I * 8u)) & 0xFFu) == u32Needle) {
			u32Count++;
		}
	}
	return u32Count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_count_bytes_eq_u - count bytes of x equal to (b & 0xFF).
 *
 * x: 32-bit word examined as four little-endian bytes (byte 0 = LSB)
 * b: target byte (only bits 7:0 used)
 *
 * Returns the number of matching bytes in [0, 4]. Example:
 * gj_u32_count_bytes_eq_u(0xAABBAABBu, 0xAAu) -> 2.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_count_bytes_eq_u(uint32_t u32X, uint32_t u32B)
{
	(void)NULL;
	return b5006_count_bytes_eq(u32X, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_count_bytes_eq_u(uint32_t u32X, uint32_t u32B)
    __attribute__((alias("gj_u32_count_bytes_eq_u")));
