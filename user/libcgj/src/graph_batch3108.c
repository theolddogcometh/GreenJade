/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3108: constant-time u64 less-than mask (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_lt_mask_u(uint64_t a, uint64_t b);
 *     - Returns all-bits-one (~0ull) if a < b (unsigned), else 0.
 *       Branch-free on a/b values. No __int128.
 *   uint64_t __gj_u64_lt_mask_u  (alias)
 *   __libcgj_batch3108_marker = "libcgj-batch3108"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_ct_lt_u64 (batch1254 0/1 form) — unique gj_u64_lt_mask_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3108_marker[] = "libcgj-batch3108";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * a < b (unsigned) → ~0ull; else 0.
 * Split into high/low 32-bit halves:
 *   a < b  iff  ah < bh  or  (ah == bh and al < bl)
 * Each half comparison uses a 64-bit subtract borrow bit (no __int128).
 */
static uint64_t
b3108_lt_mask(uint64_t uA, uint64_t uB)
{
	uint32_t uAh;
	uint32_t uAl;
	uint32_t uBh;
	uint32_t uBl;
	uint32_t uLtH;
	uint32_t uEqH;
	uint32_t uLtL;
	uint32_t uX;
	uint32_t uBit;

	uAh = (uint32_t)(uA >> 32);
	uAl = (uint32_t)uA;
	uBh = (uint32_t)(uB >> 32);
	uBl = (uint32_t)uB;

	uLtH = (uint32_t)(((uint64_t)uAh - (uint64_t)uBh) >> 63);
	uLtL = (uint32_t)(((uint64_t)uAl - (uint64_t)uBl) >> 63);

	uX = uAh ^ uBh;
	uEqH = ((uX | (0u - uX)) >> 31) ^ 1u;

	uBit = (uLtH | (uEqH & uLtL)) & 1u;
	return 0ull - (uint64_t)uBit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lt_mask_u - constant-time unsigned a < b mask (64-bit).
 *
 * a < b → 0xffffffffffffffffull; else 0. No parent wires.
 */
uint64_t
gj_u64_lt_mask_u(uint64_t uA, uint64_t uB)
{
	(void)NULL;
	return b3108_lt_mask(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_lt_mask_u(uint64_t uA, uint64_t uB)
    __attribute__((alias("gj_u64_lt_mask_u")));
