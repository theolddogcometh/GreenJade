/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2056: 64-bit mxm-style avalanche finalizer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_avalanche64(uint64_t x);
 *     - Compact 64-bit avalanche: xor high-half fold / multiply /
 *       xor / multiply / xor with public odd constant 0xd6e8feb86659fd93.
 *       Pure; no buffer walk.
 *   uint64_t __gj_hash_avalanche64  (alias)
 *   __libcgj_batch2056_marker = "libcgj-batch2056"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_mix64 (batch967 fmix64), gj_hash_finalizer64 (batch968 XXH64),
 * gj_hash_mix64_u (batch2052 moremur), and gj_splitmix64. Unique
 * gj_hash_avalanche64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2056_marker[] = "libcgj-batch2056";

/* Public odd 64-bit mxm avalanche constant. */
#define B2056_M 0xd6e8feb86659fd93ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2056_avalanche(uint64_t u64X)
{
	uint64_t u64H = u64X;

	u64H ^= u64H >> 32;
	u64H *= B2056_M;
	u64H ^= u64H >> 32;
	u64H *= B2056_M;
	u64H ^= u64H >> 32;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_avalanche64 - 64-bit avalanche finalizer of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the avalanched 64-bit result. Stateless pure function.
 */
uint64_t
gj_hash_avalanche64(uint64_t u64X)
{
	(void)NULL;
	return b2056_avalanche(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_avalanche64(uint64_t u64X)
    __attribute__((alias("gj_hash_avalanche64")));
