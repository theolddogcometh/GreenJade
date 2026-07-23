/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2328: freestanding 64-bit mx3-style final mix
 * (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_mix_final64(uint64_t x);
 *     - mx3-style 64-bit finalizer of a single 64-bit value: xor-shift /
 *       multiply stages distinct from Murmur fmix64 / moremur / XXH64.
 *       Pure; no buffer walk.
 *   uint64_t __gj_hash_mix_final64  (alias)
 *   __libcgj_batch2328_marker = "libcgj-batch2328"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from gj_hash_mix64
 * (batch967), gj_hash_finalizer64 (batch968), gj_hash_mix64_u
 * (batch2052 moremur), and gj_splitmix64 (batch424) - unique
 * gj_hash_mix_final64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public mx3
 * finalizer shape (Jonkman lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2328_marker[] = "libcgj-batch2328";

/* mx3 public finalizer constant. */
#define B2328_M 0xe9846af9b1a615dULL

/* ---- freestanding helpers ---------------------------------------------- */

/* mx3-style final mix of a single 64-bit word. */
static uint64_t
b2328_mix_final64(uint64_t u64X)
{
	uint64_t u64H = u64X;

	u64H ^= u64H >> 32;
	u64H *= B2328_M;
	u64H ^= u64H >> 32;
	u64H *= B2328_M;
	u64H ^= u64H >> 28;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix_final64 - mx3-style 64-bit finalizer mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the mixed 64-bit output. Stateless pure function.
 */
uint64_t
gj_hash_mix_final64(uint64_t u64X)
{
	(void)NULL;
	return b2328_mix_final64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_mix_final64(uint64_t u64X)
    __attribute__((alias("gj_hash_mix_final64")));
