/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch967: 64-bit MurmurHash3 finalizer mix (fmix64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_mix64(uint64_t x);
 *     — MurmurHash3 64-bit finalizer (fmix64): xor-shift / multiply /
 *       xor-shift / multiply / xor-shift. Pure; no buffer walk.
 *   uint64_t __gj_hash_mix64  (alias)
 *   __libcgj_batch967_marker = "libcgj-batch967"
 *
 * Distinct from gj_splitmix64 (batch424), gj_hash_combine_u64 (batch713),
 * and file-static b105_fmix64 (batch105, not exported). Unique
 * gj_hash_mix64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * MurmurHash3 fmix64 shape (Appleby).
 */

#include <stdint.h>

const char __libcgj_batch967_marker[] = "libcgj-batch967";

/* MurmurHash3 fmix64 constants (public algorithm). */
#define B967_M1 0xff51afd7ed558ccdULL
#define B967_M2 0xc4ceb9fe1a85ec53ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix64 — MurmurHash3-style 64-bit finalizer mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the mixed 64-bit output. Stateless pure function.
 */
uint64_t
gj_hash_mix64(uint64_t x)
{
	uint64_t u64K = x;

	u64K ^= u64K >> 33;
	u64K *= B967_M1;
	u64K ^= u64K >> 33;
	u64K *= B967_M2;
	u64K ^= u64K >> 33;
	return u64K;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_mix64(uint64_t x)
    __attribute__((alias("gj_hash_mix64")));
