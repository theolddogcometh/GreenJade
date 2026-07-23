/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3511: modular multiply for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_mul_u(uint64_t a, uint64_t b, uint64_t m);
 *     - Return (a * b) mod m without __int128. Russian-peasant
 *       accumulation with overflow-safe modular add. m == 0 → 0.
 *   uint64_t __gj_u64_mod_mul_u  (alias)
 *   __libcgj_batch3511_marker = "libcgj-batch3511"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520): u64_mod_mul_u,
 * u32_mod_inv_u, u32_mod_div_u, u32_fact_mod_u, u32_binom_mod_u,
 * u32_catalan_u, u32_fib_mod_u, u64_fib_mod_u, u32_lucas_u,
 * batch_id_3520. Unique gj_u64_mod_mul_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3511_marker[] = "libcgj-batch3511";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a + b) mod m for a, b in [0, m). m > 0.
 * Avoids intermediate overflow of a + b past UINT64_MAX.
 */
static uint64_t
b3511_addmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	/* a + b >= m  ⇔  b >= m - a  (m - a is in [1, m] when a < m). */
	if (u64B >= (u64M - u64A)) {
		return u64A - (u64M - u64B);
	}
	return u64A + u64B;
}

/*
 * (a * b) mod m. m > 0. No __int128: double-and-add over bits of b.
 * a and b are reduced into [0, m) first.
 */
static uint64_t
b3511_mulmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64R;

	u64A %= u64M;
	u64B %= u64M;
	u64R = 0u;
	while (u64B != 0u) {
		if ((u64B & 1u) != 0u) {
			u64R = b3511_addmod(u64R, u64A, u64M);
		}
		u64A = b3511_addmod(u64A, u64A, u64M);
		u64B >>= 1;
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_mul_u - modular product of two uint64_t values.
 *
 * a, b: factors
 * m:    modulus (m == 0 → 0; otherwise result in [0, m))
 *
 * Returns (a * b) mod m. Uses only 64-bit arithmetic (no __int128).
 * No parent wires.
 */
uint64_t
gj_u64_mod_mul_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;

	if (u64M == 0u) {
		return 0u;
	}
	if (u64M == 1u) {
		return 0u;
	}
	return b3511_mulmod(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_mul_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_mod_mul_u")));
