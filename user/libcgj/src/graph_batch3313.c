/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3313: portable modular exponentiation (uint64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_powmod_u2(uint64_t a, uint64_t e, uint64_t m);
 *     — compute (a^e) mod m via binary exponentiation (portable, no
 *       __int128). Renamed from gj_u64_powmod_u (collision with
 *       batch2505).
 *       Precondition m != 0; if m == 0 returns 0 (soft guard).
 *       m == 1 → 0. e == 0 → 1 when m > 1.
 *   uint64_t __gj_u64_powmod_u2  (alias)
 *   __libcgj_batch3313_marker = "libcgj-batch3313"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u64_powmod_u2 surface only; no multi-def. Distinct from
 * gj_u64_powmod_u (batch2505) and gj_powmod_u64 (batch576/230). No parent
 * wires. Portable: no __int128 / no libgcc __umodti3.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3313_marker[] = "libcgj-batch3313";

/* ---- freestanding helpers (no __int128 / no libgcc soft-quad) --------- */

/*
 * (a * b) mod m for m > 0.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 * Addition/doubling use (x >= m - y) ? (x - (m - y)) : (x + y) so
 * intermediate sums never wrap past m.
 */
static uint64_t
b3313_mulmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64R;

	u64A %= u64M;
	u64B %= u64M;
	u64R = 0ull;
	while (u64B != 0ull) {
		if ((u64B & 1ull) != 0ull) {
			uint64_t u64S = u64M - u64R;

			u64R = (u64A >= u64S) ? (u64A - u64S) : (u64R + u64A);
		}
		{
			uint64_t u64S = u64M - u64A;

			u64A = (u64A >= u64S) ? (u64A - u64S) : (u64A + u64A);
		}
		u64B >>= 1;
	}
	return u64R;
}

/* (a^e) mod m, m > 1. */
static uint64_t
b3313_powmod(uint64_t u64A, uint64_t u64E, uint64_t u64M)
{
	uint64_t u64R = 1ull;

	u64A %= u64M;
	while (u64E != 0ull) {
		if ((u64E & 1ull) != 0ull) {
			u64R = b3313_mulmod(u64R, u64A, u64M);
		}
		u64A = b3313_mulmod(u64A, u64A, u64M);
		u64E >>= 1;
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_powmod_u2 — portable (a^e) mod m (wave-unique rename).
 *
 * a: base
 * e: exponent
 * m: modulus (must be non-zero; soft 0 if m == 0)
 *
 * Special cases:
 *   m == 0  → 0  (soft guard; API documents m != 0)
 *   m == 1  → always 0
 *   e == 0  → 1 when m > 1
 *
 * Wave-unique rename: gj_u64_powmod_u is owned by batch2505.
 */
uint64_t
gj_u64_powmod_u2(uint64_t u64A, uint64_t u64E, uint64_t u64M)
{
	(void)NULL;
	if (u64M == 0ull || u64M == 1ull) {
		return 0ull;
	}
	return b3313_powmod(u64A, u64E, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_powmod_u2(uint64_t u64A, uint64_t u64E, uint64_t u64M)
    __attribute__((alias("gj_u64_powmod_u2")));
