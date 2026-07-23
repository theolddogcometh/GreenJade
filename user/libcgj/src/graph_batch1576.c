/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1576: transpose an 8x8 bit matrix in uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitmat8_transpose(uint64_t m);
 *     — Return the transpose of an 8x8 bit matrix packed in m. Layout
 *       matches gj_bitmat8_get: bit (r,c) at index r*8 + c moves to
 *       (c,r) at index c*8 + r. Pure; all inputs accepted.
 *   uint64_t __gj_bitmat8_transpose  (alias)
 *   __libcgj_batch1576_marker = "libcgj-batch1576"
 *
 * Distinct from gj_bitrev64 (batch181). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1576_marker[] = "libcgj-batch1576";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Delta-swap: exchange bits distance uShift apart where u64Mask marks the
 * higher half of each pair (left-shift form).
 */
static uint64_t
b1576_delta_swap_hi(uint64_t u64M, uint64_t u64Mask, unsigned uShift)
{
	uint64_t u64T;

	u64T = u64Mask & (u64M ^ (u64M << uShift));
	return u64M ^ u64T ^ (u64T >> uShift);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmat8_transpose — transpose 8x8 bit matrix packed in u64M.
 *
 * Three delta-swap passes (4-bit, 2-bit, 1-bit block exchanges along the
 * main diagonal). Clean-room integer path for row-major LSB packing.
 */
uint64_t
gj_bitmat8_transpose(uint64_t u64M)
{
	/* 4x4 blocks: mask 0x0F0F0F0F00000000, shift 28 */
	u64M = b1576_delta_swap_hi(u64M, 0x0F0F0F0F00000000ull, 28u);
	/* 2x2 blocks: mask 0x3333000033330000, shift 14 */
	u64M = b1576_delta_swap_hi(u64M, 0x3333000033330000ull, 14u);
	/* 1x1 bits:   mask 0x5500550055005500, shift 7 */
	u64M = b1576_delta_swap_hi(u64M, 0x5500550055005500ull, 7u);
	return u64M;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitmat8_transpose(uint64_t u64M)
    __attribute__((alias("gj_bitmat8_transpose")));
