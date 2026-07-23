/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3064: unsigned Q32.32 multiply (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_q32_mul_u(uint64_t a, uint64_t b);
 *     - Unsigned Q32.32: floor((a * b) / 2^32). Full 128-bit product is
 *       formed from 32-bit limbs (no __int128); low 64 bits of the
 *       shifted product are returned (wrap on overflow of the integer
 *       part).
 *   uint64_t __gj_q32_mul_u  (alias)
 *   __libcgj_batch3064_marker = "libcgj-batch3064"
 *
 * Q32.32 layout (unsigned 64-bit):
 *   bits 63..32 = integer part
 *   bits 31..0  = fractional part (1/2^32 units)
 *   1.0 == 0x0000000100000000ull
 *
 * Exclusive _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3064_marker[] = "libcgj-batch3064";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Low 64 bits of (a * b) >> 32 for unsigned 64-bit a,b.
 * a = a1:a0, b = b1:b0 (32-bit limbs). Product limbs:
 *   p00 = a0*b0, p01 = a0*b1, p10 = a1*b0, p11 = a1*b1
 * (a*b)>>32  (mod 2^64) = (p00>>32) + p01 + p10 + (p11<<32)  (mod 2^64)
 */
static uint64_t
b3064_mul_shr32(uint64_t u64A, uint64_t u64B)
{
	uint32_t u32A0;
	uint32_t u32A1;
	uint32_t u32B0;
	uint32_t u32B1;
	uint64_t u64P00;
	uint64_t u64P01;
	uint64_t u64P10;
	uint64_t u64P11;
	uint64_t u64Acc;
	uint64_t u64T;
	uint64_t u64Carry;

	u32A0 = (uint32_t)u64A;
	u32A1 = (uint32_t)(u64A >> 32);
	u32B0 = (uint32_t)u64B;
	u32B1 = (uint32_t)(u64B >> 32);

	u64P00 = (uint64_t)u32A0 * (uint64_t)u32B0;
	u64P01 = (uint64_t)u32A0 * (uint64_t)u32B1;
	u64P10 = (uint64_t)u32A1 * (uint64_t)u32B0;
	u64P11 = (uint64_t)u32A1 * (uint64_t)u32B1;

	u64Acc = u64P00 >> 32;
	u64Carry = 0ull;

	u64T = u64Acc + u64P01;
	if (u64T < u64Acc) {
		u64Carry++;
	}
	u64Acc = u64T;

	u64T = u64Acc + u64P10;
	if (u64T < u64Acc) {
		u64Carry++;
	}
	u64Acc = u64T;

	/* (p11 + carry) << 32 contributes to the low 64 of (prod>>32). */
	u64Acc += (u64P11 + u64Carry) << 32;
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q32_mul_u — multiply two unsigned Q32.32 values.
 *
 * Product is computed from 32-bit limbs then shifted right by 32 to
 * restore Q32.32 scale. No __int128. No parent wires.
 */
uint64_t
gj_q32_mul_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3064_mul_shr32(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_q32_mul_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_q32_mul_u")));
