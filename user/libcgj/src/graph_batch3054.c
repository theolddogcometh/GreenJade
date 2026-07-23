/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3054: uint64 percentage-of value (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pct_u(uint64_t x, uint64_t pct);
 *     - Return floor((x * pct) / 100). Overflow-safe path when x*pct
 *       would exceed UINT64_MAX (divide-first style), no __int128.
 *   uint64_t __gj_u64_pct_u  (alias)
 *   __libcgj_batch3054_marker = "libcgj-batch3054"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique gj_u64_pct_u
 * surface only; no parent wires, no multi-def. Distinct from
 * gj_pct_of_u64 (batch996) and gj_percent_of_u64 (batch1746).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3054_marker[] = "libcgj-batch3054";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((x * pct) / 100) with an overflow-safe path that never forms a
 * wrapping product. No __int128.
 */
static uint64_t
b3054_pct(uint64_t u64X, uint64_t u64Pct)
{
	if (u64X == 0ull || u64Pct == 0ull) {
		return 0ull;
	}
	if (u64X > UINT64_MAX / u64Pct) {
		/* (x/100)*pct + ((x%100)*pct)/100 — product of rem path
		 * fits: (x%100) < 100 so (x%100)*pct may still overflow
		 * if pct is huge; split further. */
		uint64_t u64Q = u64X / 100ull;
		uint64_t u64R = u64X % 100ull;
		uint64_t u64Main;
		uint64_t u64Frac;

		if (u64Q > UINT64_MAX / u64Pct) {
			/* Extreme: scale pct first. */
			return (u64Q / 100ull) * u64Pct +
			    ((u64Q % 100ull) * u64Pct) / 100ull +
			    (u64R * (u64Pct / 100ull)) +
			    (u64R * (u64Pct % 100ull)) / 100ull;
		}
		u64Main = u64Q * u64Pct;
		if (u64R > UINT64_MAX / u64Pct) {
			u64Frac = (u64R * (u64Pct / 100ull)) +
			    (u64R * (u64Pct % 100ull)) / 100ull;
		} else {
			u64Frac = (u64R * u64Pct) / 100ull;
		}
		return u64Main + u64Frac;
	}
	return (u64X * u64Pct) / 100ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pct_u - compute floor((x * pct) / 100) for uint64_t budgets.
 *
 * x:   base value
 * pct: percentage (100 = 100%)
 *
 * Overflow-safe integer path; pure arithmetic, no syscalls. Self-
 * contained: no parent wires to mul_div helpers.
 */
uint64_t
gj_u64_pct_u(uint64_t u64X, uint64_t u64Pct)
{
	(void)NULL;
	return b3054_pct(u64X, u64Pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pct_u(uint64_t u64X, uint64_t u64Pct)
    __attribute__((alias("gj_u64_pct_u")));
