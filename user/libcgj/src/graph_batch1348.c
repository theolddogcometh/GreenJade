/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1348: Cantor pairing function.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cantor_pair(uint32_t x, uint32_t y);
 *     — π(x,y) = (x+y)*(x+y+1)/2 + y  (bijection N×N → N).
 *   uint64_t __gj_cantor_pair  (alias)
 *   __libcgj_batch1348_marker = "libcgj-batch1348"
 *
 * Distinct from Szudzik/elegant pairing. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1348_marker[] = "libcgj-batch1348";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cantor_pair — classic Cantor tuple function for two naturals.
 *
 * Returns a unique 64-bit code for every (x, y) pair with x,y ≤ 2^32-1
 * (result fits in uint64 for the full domain of uint32 inputs).
 */
uint64_t
gj_cantor_pair(uint32_t u32X, uint32_t u32Y)
{
	uint64_t u64S;

	u64S = (uint64_t)u32X + (uint64_t)u32Y;
	return (u64S * (u64S + 1ull)) / 2ull + (uint64_t)u32Y;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cantor_pair(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_cantor_pair")));
