/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4003: extract the MSB / sign bit of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_signbit_u(uint32_t x);
 *     - Return 1 if bit 31 (MSB) of x is set, else 0. Treats the word
 *       as a bit pattern (two's-complement / IEEE sign position).
 *   uint32_t __gj_u32_signbit_u  (alias)
 *   __libcgj_batch4003_marker = "libcgj-batch4003"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_f32_bits_sign (batch1834) — unique gj_u32_signbit_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4003_marker[] = "libcgj-batch4003";

/* uint32_t MSB / two's-complement sign position. */
#define B4003_MSB  ((uint32_t)0x80000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4003_signbit(uint32_t u32X)
{
	if ((u32X & B4003_MSB) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_signbit_u - 1 if bit 31 of x is set, else 0.
 *
 * x: 32-bit word (bit pattern)
 *
 * Returns 1 when the MSB is set, else 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_u32_signbit_u(uint32_t u32X)
{
	(void)NULL;
	return b4003_signbit(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_signbit_u(uint32_t u32X)
    __attribute__((alias("gj_u32_signbit_u")));
