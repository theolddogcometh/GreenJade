/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5004: test whether any byte of a uint64_t is zero.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_has_zero_byte_u(uint64_t x);
 *     - Return 1 if any of the 8 bytes of x is 0x00, else 0.
 *   uint32_t __gj_u64_has_zero_byte_u  (alias)
 *   __libcgj_batch5004_marker = "libcgj-batch5004"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010). Distinct from
 * gj_u32_has_zero_byte_u (batch5003) — unique gj_u64_has_zero_byte_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5004_marker[] = "libcgj-batch5004";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5004_has_zero_byte(uint64_t u64X)
{
	uint32_t u32I;

	for (u32I = 0u; u32I < 8u; u32I++) {
		if (((u64X >> (u32I * 8u)) & 0xFFull) == 0ull) {
			return 1u;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_has_zero_byte_u - 1 if any byte of x is zero, else 0.
 *
 * x: 64-bit word examined as eight little-endian bytes (byte 0 = LSB)
 *
 * Returns 1 when any of the eight bytes is 0x00, else 0. Example:
 * gj_u64_has_zero_byte_u(0x0011223344556677ull) -> 1.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u64_has_zero_byte_u(uint64_t u64X)
{
	(void)NULL;
	return b5004_has_zero_byte(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_has_zero_byte_u(uint64_t u64X)
    __attribute__((alias("gj_u64_has_zero_byte_u")));
