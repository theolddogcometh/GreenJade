/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4715: 64-bit hash finalizer mix cascade (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_mix_final64_u(uint64_t x);
 *     - Avalanche finalizer for a single 64-bit hash accumulator:
 *       xorshift / odd-multiply cascade unique to this TU. Pure; no
 *       buffer walk.
 *   uint64_t __gj_hash_mix_final64_u  (alias)
 *   __libcgj_batch4715_marker = "libcgj-batch4715"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique
 * gj_hash_mix_final64_u surface only; no multi-def. Distinct from
 * gj_hash_mix_final64 (batch2328 mx3), gj_hash_mix64 (batch967
 * Murmur fmix64), gj_u64_mix_final_u (batch4026), and
 * gj_hash_finalizer64 (batch968 XXH). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4715_marker[] = "libcgj-batch4715";

/* Cascade-unique 64-bit finalizer multipliers (not murmur/mx3/xxh). */
#define B4715_C1  0xe7037ed1a0b428dbULL
#define B4715_C2  0x8e38b7f7c3e0f5a5ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4715_mix_final64(uint64_t u64X)
{
	u64X ^= u64X >> 30;
	u64X *= B4715_C1;
	u64X ^= u64X >> 27;
	u64X *= B4715_C2;
	u64X ^= u64X >> 31;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix_final64_u - 64-bit cascade finalizer mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns a fully diffused 64-bit value. Stateless pure function.
 * No parent wires.
 */
uint64_t
gj_hash_mix_final64_u(uint64_t u64X)
{
	(void)NULL;
	return b4715_mix_final64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_mix_final64_u(uint64_t u64X)
    __attribute__((alias("gj_hash_mix_final64_u")));
