/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8435: CRC bit-reflect integer stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc_reflect_u_8435(uint32_t x, uint32_t width);
 *     - Reflect the low width bits of x (bit i <-> bit width-1-i).
 *       width 0 or >32 → 0. High bits above width are discarded.
 *   uint32_t __gj_crc_reflect_u_8435  (alias)
 *   __libcgj_batch8435_marker = "libcgj-batch8435"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc_reflect_u_8435 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8435_marker[] = "libcgj-batch8435";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reflect low u32Width bits of u32X; 0 if width out of 1..32. */
static uint32_t
b8435_reflect(uint32_t u32X, uint32_t u32Width)
{
	uint32_t u32Out;
	uint32_t u32I;

	if (u32Width == 0u || u32Width > 32u) {
		return 0u;
	}

	u32Out = 0u;
	for (u32I = 0u; u32I < u32Width; u32I++) {
		if ((u32X >> u32I) & 1u) {
			u32Out |= (uint32_t)(1u << (u32Width - 1u - u32I));
		}
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc_reflect_u_8435 - bit-reflect x over width bits.
 *
 * x: value to reflect
 * width: number of low bits to reverse (1..32); else 0
 *
 * Returns the reflected low-width field. Soft integer helper for
 * refin/refout CRC paths. No parent wires.
 */
uint32_t
gj_crc_reflect_u_8435(uint32_t u32X, uint32_t u32Width)
{
	(void)NULL;
	return b8435_reflect(u32X, u32Width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc_reflect_u_8435(uint32_t u32X, uint32_t u32Width)
    __attribute__((alias("gj_crc_reflect_u_8435")));
