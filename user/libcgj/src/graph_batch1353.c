/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1353: elegant (exponential) pairing function.
 *
 * Surface (unique symbols):
 *   uint64_t gj_elegant_pair(uint32_t x, uint32_t y);
 *     — π(x,y) = 2^x * (2*y + 1) - 1  (bijection N×N → N).
 *       Returns UINT64_MAX if 2^x would overflow the intermediate.
 *   uint64_t __gj_elegant_pair  (alias)
 *   __libcgj_batch1353_marker = "libcgj-batch1353"
 *
 * Distinct from Cantor (batch1348) and Szudzik (batch1351). Unique
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1353_marker[] = "libcgj-batch1353";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elegant_pair — exponential elegant pairing.
 *
 * Unique code for each (x, y). x must be < 63 for a finite non-saturated
 * result when y is large; on overflow of the product intermediate,
 * returns UINT64_MAX as a sentinel.
 */
uint64_t
gj_elegant_pair(uint32_t u32X, uint32_t u32Y)
{
	uint64_t u64Pow;
	uint64_t u64Odd;
	uint64_t u64Prod;

	if (u32X >= 63u) {
		return UINT64_MAX;
	}
	u64Pow = 1ull << u32X;
	u64Odd = 2ull * (uint64_t)u32Y + 1ull;
	/* Check u64Pow * u64Odd overflow before multiply. */
	if (u64Odd != 0ull && u64Pow > UINT64_MAX / u64Odd) {
		return UINT64_MAX;
	}
	u64Prod = u64Pow * u64Odd;
	if (u64Prod == 0ull) {
		return UINT64_MAX;
	}
	return u64Prod - 1ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_elegant_pair(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_elegant_pair")));
