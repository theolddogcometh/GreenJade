/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2058: 64-bit Fibonacci multiplicative hash step.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_fib_mul64(uint64_t x);
 *     - Multiply x by the 64-bit fractional golden-ratio odd constant
 *       0x9e3779b97f4a7c15 (2^64 / phi). Classic Fibonacci hashing
 *       multiplier. Pure; no buffer walk.
 *   uint64_t __gj_hash_fib_mul64  (alias)
 *   __libcgj_batch2058_marker = "libcgj-batch2058"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_fib_mul32 (batch2057), Boost combine additives, and
 * splitmix/fmix finalizers - multiply-only Fibonacci hashing. Unique
 * gj_hash_fib_mul64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2058_marker[] = "libcgj-batch2058";

/* 2^64 / phi fractional part (public Fibonacci hash multiplier). */
#define B2058_FIB 0x9e3779b97f4a7c15ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2058_fib_mul(uint64_t u64X)
{
	return u64X * B2058_FIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_fib_mul64 - Fibonacci multiplicative mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns x * 0x9e3779b97f4a7c15. Callers that need a table index
 * typically shift the high bits: (result >> (64 - bits)). Stateless
 * pure function.
 */
uint64_t
gj_hash_fib_mul64(uint64_t u64X)
{
	(void)NULL;
	return b2058_fib_mul(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_fib_mul64(uint64_t u64X)
    __attribute__((alias("gj_hash_fib_mul64")));
