/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3317: Legendre symbol on uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_legendre_u(uint32_t a, uint32_t p);
 *     — Legendre symbol (a/p) via modular exponentiation:
 *       a^((p-1)/2) mod p ∈ {0, 1, p-1}. Maps p-1 → -1.
 *       Requires p odd and p > 2 (caller should pass an odd prime).
 *       p even or p <= 2 → 0 (soft invalid). a is reduced mod p.
 *   int __gj_u32_legendre_u  (alias)
 *   __libcgj_batch3317_marker = "libcgj-batch3317"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u32_legendre_u surface only; no multi-def. Distinct from
 * gj_u32_jacobi_u (batch3316) — Euler criterion implementation; no parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3317_marker[] = "libcgj-batch3317";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3317_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* (a^e) mod m, m > 1. */
static uint32_t
b3317_powmod(uint32_t u32A, uint32_t u32E, uint32_t u32M)
{
	uint32_t u32R = 1u;

	u32A %= u32M;
	while (u32E != 0u) {
		if ((u32E & 1u) != 0u) {
			u32R = b3317_mulmod(u32R, u32A, u32M);
		}
		u32A = b3317_mulmod(u32A, u32A, u32M);
		u32E >>= 1;
	}
	return u32R;
}

/*
 * Legendre via Euler criterion: (a/p) ≡ a^((p-1)/2) (mod p).
 * Preconditions: p odd, p > 2.
 */
static int
b3317_legendre(uint32_t u32A, uint32_t u32P)
{
	uint32_t u32R;

	if ((u32P & 1u) == 0u || u32P <= 2u) {
		return 0;
	}

	u32A %= u32P;
	if (u32A == 0u) {
		return 0;
	}

	u32R = b3317_powmod(u32A, (u32P - 1u) / 2u, u32P);
	if (u32R == 0u || u32R == 1u) {
		return (int)u32R;
	}
	if (u32R == u32P - 1u) {
		return -1;
	}
	/* Non-prime p may yield other residues; treat as 0. */
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_legendre_u — Legendre symbol (a/p).
 *
 * a: numerator (reduced mod p)
 * p: odd prime denominator (even / <=2 → 0 soft invalid)
 *
 * Returns -1, 0, or +1 when p is an odd prime.
 */
int
gj_u32_legendre_u(uint32_t u32A, uint32_t u32P)
{
	(void)NULL;
	return b3317_legendre(u32A, u32P);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_legendre_u(uint32_t u32A, uint32_t u32P)
    __attribute__((alias("gj_u32_legendre_u")));
