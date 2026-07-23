/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8253: count-trailing-zeros integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_ctz_u_8253(uint32_t v);
 *     - Number of consecutive 0-bits starting from bit 0 (LSB).
 *       Returns 0 when v == 0 (defined convention for this surface).
 *   uint32_t __gj_bit_ctz_u_8253  (alias)
 *   __libcgj_batch8253_marker = "libcgj-batch8253"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_ctz_u_8253 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8253_marker[] = "libcgj-batch8253";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8253_ctz — count trailing zeros of a 32-bit word (0 if zero).
 * Binary-search peel of 16/8/4/2/1 with low half-masks.
 */
static uint32_t
b8253_ctz(uint32_t u32V)
{
	uint32_t u32X;
	uint32_t u32Ctz;

	if (u32V == 0u) {
		return 0u;
	}

	u32X = u32V;
	u32Ctz = 0u;

	if ((u32X & 0xffffu) == 0u) {
		u32Ctz += 16u;
		u32X >>= 16;
	}
	if ((u32X & 0xffu) == 0u) {
		u32Ctz += 8u;
		u32X >>= 8;
	}
	if ((u32X & 0xfu) == 0u) {
		u32Ctz += 4u;
		u32X >>= 4;
	}
	if ((u32X & 0x3u) == 0u) {
		u32Ctz += 2u;
		u32X >>= 2;
	}
	if ((u32X & 0x1u) == 0u) {
		u32Ctz += 1u;
	}

	return u32Ctz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_ctz_u_8253 - count trailing zero bits in a 32-bit word.
 *
 * v: value to inspect
 *
 * Returns the number of consecutive 0-bits from the LSB. Zero input is
 * defined as 0 for this surface. Soft ctz catalog surface; pure integer,
 * no builtins. No parent wires.
 */
uint32_t
gj_bit_ctz_u_8253(uint32_t u32V)
{
	(void)NULL;
	return b8253_ctz(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_ctz_u_8253(uint32_t u32V)
    __attribute__((alias("gj_bit_ctz_u_8253")));
