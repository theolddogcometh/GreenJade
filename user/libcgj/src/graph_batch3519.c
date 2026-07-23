/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3519: Lucas number L_n as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lucas_u(uint32_t n);
 *     - Return the n-th Lucas number L_n.
 *       L_0 = 2, L_1 = 1, L_n = L_{n-1} + L_{n-2}.
 *       Exact for n <= 46 (L_46 = 4106118243 fits uint32_t).
 *       Returns 0 for n >= 47 (overflow past UINT32_MAX).
 *   uint32_t __gj_u32_lucas_u  (alias)
 *   __libcgj_batch3519_marker = "libcgj-batch3519"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_lucas_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3519_marker[] = "libcgj-batch3519";

/* L_n fits in uint32_t for n in 0..46 inclusive. */
#define B3519_LUCAS_U32_MAX_N  46u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Iterative Lucas: L_0=2, L_1=1. Tracks previous two terms in uint64
 * to detect overflow past UINT32_MAX cleanly (caller already bounds n).
 */
static uint32_t
b3519_lucas(uint32_t u32N)
{
	uint64_t u64A;
	uint64_t u64B;
	uint32_t u32I;

	if (u32N > B3519_LUCAS_U32_MAX_N) {
		return 0u;
	}
	if (u32N == 0u) {
		return 2u;
	}
	if (u32N == 1u) {
		return 1u;
	}

	u64A = 2u; /* L_0 */
	u64B = 1u; /* L_1 */
	for (u32I = 2u; u32I <= u32N; u32I++) {
		uint64_t u64C = u64A + u64B;

		u64A = u64B;
		u64B = u64C;
	}
	return (uint32_t)u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lucas_u - n-th Lucas number as uint32_t.
 *
 * n: index (L_0=2, L_1=1)
 *
 * Returns L_n for n <= 46; 0 for n >= 47 (does not fit uint32_t).
 * No parent wires.
 */
uint32_t
gj_u32_lucas_u(uint32_t u32N)
{
	(void)NULL;
	return b3519_lucas(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lucas_u(uint32_t u32N)
    __attribute__((alias("gj_u32_lucas_u")));
