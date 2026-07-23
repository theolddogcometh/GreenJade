/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3914: uint32 coprime predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_coprime_u(uint32_t a, uint32_t b);
 *     - Return 1 if gcd(a, b) == 1 (a and b are coprime), else 0.
 *       Local Euclidean GCD only (no parent wires to gj_gcd_u32).
 *   int __gj_u32_coprime_u  (alias)
 *   __libcgj_batch3914_marker = "libcgj-batch3914"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_coprime_u surface only; no multi-def. Distinct from
 * gj_u64_coprime_u (batch3915). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3914_marker[] = "libcgj-batch3914";

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean gcd; gcd(0,0)=0; gcd(x,0)=x. */
static uint32_t
b3914_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;

		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* 1 if gcd(a,b) == 1. */
static int
b3914_coprime(uint32_t u32A, uint32_t u32B)
{
	return b3914_gcd(u32A, u32B) == 1u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_coprime_u - true when a and b are coprime (gcd == 1).
 *
 * a, b: unsigned operands
 * No parent wires.
 */
int
gj_u32_coprime_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3914_coprime(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_coprime_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_coprime_u")));
