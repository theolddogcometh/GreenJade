/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3915: uint64 coprime predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_coprime_u(uint64_t a, uint64_t b);
 *     - Return 1 if gcd(a, b) == 1 (a and b are coprime), else 0.
 *       Local Euclidean GCD only (no parent wires to gj_gcd_u64).
 *   int __gj_u64_coprime_u  (alias)
 *   __libcgj_batch3915_marker = "libcgj-batch3915"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u64_coprime_u surface only; no multi-def. Distinct from
 * gj_u32_coprime_u (batch3914). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3915_marker[] = "libcgj-batch3915";

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean gcd; gcd(0,0)=0; gcd(x,0)=x. */
static uint64_t
b3915_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ull) {
		uint64_t u64T = u64A % u64B;

		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/* 1 if gcd(a,b) == 1. */
static int
b3915_coprime(uint64_t u64A, uint64_t u64B)
{
	return b3915_gcd(u64A, u64B) == 1ull ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_coprime_u - true when a and b are coprime (gcd == 1).
 *
 * a, b: unsigned operands
 * No parent wires.
 */
int
gj_u64_coprime_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3915_coprime(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_coprime_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_coprime_u")));
