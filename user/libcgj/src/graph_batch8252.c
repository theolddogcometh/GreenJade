/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8252: count-leading-zeros integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_clz_u_8252(uint32_t v);
 *     - Number of consecutive 0-bits starting from bit 31 (MSB).
 *       Returns 32 when v == 0 (fully defined for zero input).
 *   uint32_t __gj_bit_clz_u_8252  (alias)
 *   __libcgj_batch8252_marker = "libcgj-batch8252"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_clz_u_8252 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8252_marker[] = "libcgj-batch8252";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8252_clz — count leading zeros of a 32-bit word (32 if zero).
 * Binary-search peel of 16/8/4/2/1 against high half-masks.
 */
static uint32_t
b8252_clz(uint32_t u32V)
{
	uint32_t u32X;
	uint32_t u32Clz;

	if (u32V == 0u) {
		return 32u;
	}

	u32X = u32V;
	u32Clz = 0u;

	if ((u32X & 0xffff0000u) == 0u) {
		u32Clz += 16u;
		u32X <<= 16;
	}
	if ((u32X & 0xff000000u) == 0u) {
		u32Clz += 8u;
		u32X <<= 8;
	}
	if ((u32X & 0xf0000000u) == 0u) {
		u32Clz += 4u;
		u32X <<= 4;
	}
	if ((u32X & 0xc0000000u) == 0u) {
		u32Clz += 2u;
		u32X <<= 2;
	}
	if ((u32X & 0x80000000u) == 0u) {
		u32Clz += 1u;
	}

	return u32Clz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_clz_u_8252 - count leading zero bits in a 32-bit word.
 *
 * v: value to inspect
 *
 * Returns the number of consecutive 0-bits from the MSB. Zero input is
 * defined as 32. Soft clz catalog surface; pure integer, no builtins.
 * No parent wires.
 */
uint32_t
gj_bit_clz_u_8252(uint32_t u32V)
{
	(void)NULL;
	return b8252_clz(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_clz_u_8252(uint32_t u32V)
    __attribute__((alias("gj_bit_clz_u_8252")));
