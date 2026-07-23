/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch230: modular exponentiation for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_modpow_u64(uint64_t base, uint64_t exp, uint64_t mod);
 *     — compute (base^exp) mod mod via binary exponentiation.
 *       mod==0 means natural uint64 wrap (mod 2^64).
 *       mod==1 → 0. exp==0 → 1 (or 0 when mod==1).
 *   __gj_modpow_u64  (alias)
 *   __libcgj_batch230_marker = "libcgj-batch230"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No __uint128_t / no libgcc __umodti3. No malloc,
 * no errno. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch230_marker[] = "libcgj-batch230";

/* ---- modular helpers (no __uint128_t / no libgcc __umodti3) ------------ */

/*
 * (a * b) mod m. When m==0, returns a*b with uint64 wrap.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 */
static uint64_t
b230_mulmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t r;

	if (m == 0u) {
		return a * b;
	}
	a %= m;
	b %= m;
	r = 0u;
	while (b != 0u) {
		if ((b & 1u) != 0u) {
			uint64_t s = m - r;

			r = (a >= s) ? (a - s) : (r + a);
		}
		{
			uint64_t s = m - a;

			a = (a >= s) ? (a - s) : (a + a);
		}
		b >>= 1;
	}
	return r;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_modpow_u64 — modular power (base^exp) mod mod.
 *
 * Binary (square-and-multiply) exponentiation with mulmod that never
 * widens to 128 bits, so freestanding -ffreestanding links without
 * libgcc soft-quad helpers.
 *
 * Special cases:
 *   mod == 0  → arithmetic mod 2^64 (uint64 wrap on multiply)
 *   mod == 1  → always 0
 *   exp == 0  → 1 % mod  (i.e. 1 when mod > 1 or mod == 0; 0 when mod == 1)
 */
uint64_t
gj_modpow_u64(uint64_t base, uint64_t exp, uint64_t mod)
{
	uint64_t result;

	if (mod == 1u) {
		return 0u;
	}

	result = 1u;
	if (mod != 0u) {
		base %= mod;
	}

	while (exp != 0u) {
		if ((exp & 1u) != 0u) {
			result = b230_mulmod(result, base, mod);
		}
		base = b230_mulmod(base, base, mod);
		exp >>= 1;
	}
	return result;
}

uint64_t __gj_modpow_u64(uint64_t base, uint64_t exp, uint64_t mod)
    __attribute__((alias("gj_modpow_u64")));
