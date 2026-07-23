/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4167: widen a uint32_t, mix in u64, fold to u32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_hash_mix64_u(uint32_t x);
 *     - Zero-extend x to uint64_t, apply a SplitMix64-style finalizer
 *       mix (xor-shift / multiply stages), then fold hi^lo to uint32_t.
 *       All inputs accepted including 0.
 *   uint32_t __gj_u32_hash_mix64_u  (alias)
 *   __libcgj_batch4167_marker = "libcgj-batch4167"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_u32_hash_mix64_u surface only; no multi-def. Distinct from
 * gj_hash_mix64 (batch967 fmix64), gj_hash_mix64_u (batch2052 moremur),
 * gj_hash_mix32_u (batch2051). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4167_marker[] = "libcgj-batch4167";

/* SplitMix64 finalizer constant (public algorithm lineage). */
#define B4167_SMIX 0x9e3779b97f4a7c15ULL
#define B4167_M1   0xbf58476d1ce4e5b9ULL
#define B4167_M2   0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Widen x to 64 bits, avalanche-mix, fold hi^lo to 32 bits.
 * Pure integer; no builtins, no __int128.
 */
static uint32_t
b4167_mix64_fold(uint32_t u32X)
{
	uint64_t u64Z;

	u64Z = (uint64_t)u32X;
	u64Z += B4167_SMIX;
	u64Z = (u64Z ^ (u64Z >> 30)) * B4167_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B4167_M2;
	u64Z ^= u64Z >> 31;
	return (uint32_t)(u64Z ^ (u64Z >> 32));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_hash_mix64_u - widen-mix-fold a uint32_t into a mixed uint32_t.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the folded 32-bit digest after a 64-bit mix stage. Stateless
 * pure function. No parent wires.
 */
uint32_t
gj_u32_hash_mix64_u(uint32_t u32X)
{
	(void)NULL;
	return b4167_mix64_fold(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_hash_mix64_u(uint32_t u32X)
    __attribute__((alias("gj_u32_hash_mix64_u")));
