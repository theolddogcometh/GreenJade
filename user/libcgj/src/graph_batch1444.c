/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1444: NASAM 64-bit mixer (Pelle Evensen).
 *
 * Surface (unique symbols):
 *   uint64_t gj_nasam_mix_u64(uint64_t x);
 *     — NASAM avalanche of a single u64: rotate-xor / multiply /
 *       dual xor-shift / multiply / dual xor-shift. Pure; no buffer.
 *   uint64_t __gj_nasam_mix_u64  (alias)
 *   __libcgj_batch1444_marker = "libcgj-batch1444"
 *
 * Distinct from gj_rrmxmx_u64 (batch1443), gj_murmur_fmix64,
 * gj_wyhash64_mix, and gj_hash_finalizer64. Unique surface only; no
 * multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public NASAM
 * mixer shape.
 */

#include <stdint.h>

const char __libcgj_batch1444_marker[] = "libcgj-batch1444";

/* NASAM public odd multiply constants. */
#define B1444_M1 0x9e6c63d0676a9a99ULL
#define B1444_M2 0x9e6d62d06f6a9a9bULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1444_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nasam_mix_u64 — NASAM mixer of a single 64-bit word.
 *
 * u64X: arbitrary input (all values accepted, including 0).
 *
 * Public shape:
 *   x ^= rotl(x, 25) ^ rotl(x, 47);
 *   x *= M1; x ^= (x >> 23) ^ (x >> 51);
 *   x *= M2; x ^= (x >> 23) ^ (x >> 51);
 */
uint64_t
gj_nasam_mix_u64(uint64_t u64X)
{
	uint64_t u64V = u64X;

	u64V ^= b1444_rotl64(u64V, 25u) ^ b1444_rotl64(u64V, 47u);
	u64V *= B1444_M1;
	u64V ^= (u64V >> 23) ^ (u64V >> 51);
	u64V *= B1444_M2;
	u64V ^= (u64V >> 23) ^ (u64V >> 51);
	return u64V;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_nasam_mix_u64(uint64_t u64X)
    __attribute__((alias("gj_nasam_mix_u64")));
