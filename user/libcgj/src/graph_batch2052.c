/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2052: 64-bit moremur-style finalizer mix (_u wave).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_mix64_u(uint64_t x);
 *     - Moremur-style 64-bit finalizer: xor-shift / multiply stages that
 *       are distinct from Murmur fmix64. Pure; no buffer walk.
 *   uint64_t __gj_hash_mix64_u  (alias)
 *   __libcgj_batch2052_marker = "libcgj-batch2052"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_mix64 (batch967 Murmur fmix64), gj_hash_finalizer64 (batch968),
 * and gj_splitmix64 (batch424). Unique gj_hash_mix64_u surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public moremur
 * mixer shape (Stafford / Evensen lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2052_marker[] = "libcgj-batch2052";

/* Moremur public mixing constants. */
#define B2052_M1 0x3C79AC492BA7B653ULL
#define B2052_M2 0x1C69B3F74AC4AE35ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2052_mix(uint64_t u64X)
{
	uint64_t u64H = u64X;

	u64H ^= u64H >> 27;
	u64H *= B2052_M1;
	u64H ^= u64H >> 33;
	u64H *= B2052_M2;
	u64H ^= u64H >> 27;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix64_u - moremur-style 64-bit finalizer mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the mixed 64-bit output. Stateless pure function.
 */
uint64_t
gj_hash_mix64_u(uint64_t u64X)
{
	(void)NULL;
	return b2052_mix(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_mix64_u(uint64_t u64X)
    __attribute__((alias("gj_hash_mix64_u")));
