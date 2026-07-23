/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2057: 32-bit Fibonacci multiplicative hash step.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_fib_mul32(uint32_t x);
 *     - Multiply x by the 32-bit fractional golden-ratio odd constant
 *       0x9e3779b9 (2^32 / phi). Classic Fibonacci hashing multiplier.
 *       Pure; no buffer walk.
 *   uint32_t __gj_hash_fib_mul32  (alias)
 *   __libcgj_batch2057_marker = "libcgj-batch2057"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from Boost
 * combine additives and fmix/avalanche finalizers - this is multiply-only
 * Fibonacci hashing. Unique gj_hash_fib_mul32 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2057_marker[] = "libcgj-batch2057";

/* 2^32 / phi fractional part (public Fibonacci hash multiplier). */
#define B2057_FIB 0x9e3779b9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2057_fib_mul(uint32_t u32X)
{
	return u32X * B2057_FIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fib_mul32 - Fibonacci multiplicative mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns x * 0x9e3779b9. Callers that need a table index typically
 * shift the high bits: (result >> (32 - bits)). Stateless pure function.
 */
uint32_t
gj_hash_fib_mul32(uint32_t u32X)
{
	(void)NULL;
	return b2057_fib_mul(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_fib_mul32(uint32_t u32X)
    __attribute__((alias("gj_hash_fib_mul32")));
