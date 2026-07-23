/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1254: constant-time unsigned less-than (u64).
 *
 * Surface (unique symbols):
 *   int gj_ct_lt_u64(uint64_t a, uint64_t b);
 *     — 1 if a < b (unsigned), else 0; branch-free on a/b values.
 *   int __gj_ct_lt_u64  (alias)
 *   __libcgj_batch1254_marker = "libcgj-batch1254"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1254_marker[] = "libcgj-batch1254";

/*
 * gj_ct_lt_u64 — constant-time unsigned a < b.
 *
 * Splits into high/low 32-bit halves and combines:
 *   a < b  iff  ah < bh  or  (ah == bh and al < bl)
 * Each half comparison uses a 64-bit subtract borrow bit.
 */
int
gj_ct_lt_u64(uint64_t u64A, uint64_t u64B)
{
	uint32_t u32Ah;
	uint32_t u32Al;
	uint32_t u32Bh;
	uint32_t u32Bl;
	uint32_t u32LtH;
	uint32_t u32EqH;
	uint32_t u32LtL;
	uint32_t u32X;

	u32Ah = (uint32_t)(u64A >> 32);
	u32Al = (uint32_t)u64A;
	u32Bh = (uint32_t)(u64B >> 32);
	u32Bl = (uint32_t)u64B;

	/* (x < y) for uint32 via borrow of x - y in 64-bit. */
	u32LtH = (uint32_t)(((uint64_t)u32Ah - (uint64_t)u32Bh) >> 63);
	u32LtL = (uint32_t)(((uint64_t)u32Al - (uint64_t)u32Bl) >> 63);

	/* (ah == bh): 1 if XOR is zero. */
	u32X = u32Ah ^ u32Bh;
	u32EqH = ((u32X | (0u - u32X)) >> 31) ^ 1u;

	return (int)((u32LtH | (u32EqH & u32LtL)) & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ct_lt_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ct_lt_u64")));
