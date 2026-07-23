/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1735: uint64_t powers of ten for fixed print.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pow10(unsigned exp);
 *     — Returns 10^exp for exp in [0, 19]. Returns 0 when exp > 19
 *       (overflow beyond uint64_t; 10^19 still fits).
 *   uint64_t __gj_u64_pow10  (alias)
 *   __libcgj_batch1735_marker = "libcgj-batch1735"
 *
 * Decimal/fixed-print exclusive. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1735_marker[] = "libcgj-batch1735";

/* ---- freestanding helpers ---------------------------------------------- */

/* Exact 10^k for k = 0..19 (fits in uint64_t). */
static const uint64_t g_b1735_pow10[20] = {
	1ULL,
	10ULL,
	100ULL,
	1000ULL,
	10000ULL,
	100000ULL,
	1000000ULL,
	10000000ULL,
	100000000ULL,
	1000000000ULL,
	10000000000ULL,
	100000000000ULL,
	1000000000000ULL,
	10000000000000ULL,
	100000000000000ULL,
	1000000000000000ULL,
	10000000000000000ULL,
	100000000000000000ULL,
	1000000000000000000ULL,
	10000000000000000000ULL
};

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pow10 — 10^exp, or 0 if exp exceeds 19.
 */
uint64_t
gj_u64_pow10(unsigned uExp)
{
	if (uExp > 19u) {
		return 0ull;
	}
	return g_b1735_pow10[uExp];
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pow10(unsigned uExp)
    __attribute__((alias("gj_u64_pow10")));
