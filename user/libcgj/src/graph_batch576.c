/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch576: modular exponentiation for uint64_t.
 *
 * Surface (unique symbols for this TU):
 *   uint64_t gj_powmod_u64(uint64_t base, uint64_t exp, uint64_t mod);
 *     — compute (base^exp) mod mod via binary exponentiation.
 *       mod==0 → 0. mod==1 → 0. exp==0 → 1 (or 0 when mod==1 or mod==0).
 *   __gj_powmod_u64  (alias)
 *   __libcgj_batch576_marker = "libcgj-batch576"
 *
 * Avoid multi-def: gj_powmod_u64 / __gj_powmod_u64 also live in
 * graph_batch230.c (different mod==0 semantics there: uint64 wrap).
 * Wire only one of batch230 or batch576 into the library build.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No __uint128_t / no
 * libgcc __umodti3. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stdint.h>

const char __libcgj_batch576_marker[] = "libcgj-batch576";

/* ---- modular helpers (no __uint128_t / no libgcc __umodti3) ------------ */

/*
 * (a * b) mod m for m > 0.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 * Addition/doubling use (x >= m - y) ? (x - (m - y)) : (x + y) so
 * intermediate sums never wrap past m.
 */
static uint64_t
b576_mulmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t r;

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
 * gj_powmod_u64 — modular power (base^exp) mod mod.
 *
 * Binary (square-and-multiply) exponentiation with mulmod that never
 * widens to 128 bits, so freestanding -ffreestanding links without
 * libgcc soft-quad helpers.
 *
 * Special cases:
 *   mod == 0  → 0
 *   mod == 1  → always 0
 *   exp == 0  → 1 % mod  (i.e. 1 when mod > 1; 0 when mod == 0 or 1)
 */
uint64_t
gj_powmod_u64(uint64_t base, uint64_t exp, uint64_t mod)
{
	uint64_t result;

	if (mod == 0u || mod == 1u) {
		return 0u;
	}

	result = 1u;
	base %= mod;

	while (exp != 0u) {
		if ((exp & 1u) != 0u) {
			result = b576_mulmod(result, base, mod);
		}
		base = b576_mulmod(base, base, mod);
		exp >>= 1;
	}
	return result;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_powmod_u64(uint64_t base, uint64_t exp, uint64_t mod)
    __attribute__((alias("gj_powmod_u64")));
