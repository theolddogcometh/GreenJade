/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1442: MurmurHash3 64-bit finalizer (fmix64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_murmur_fmix64(uint64_t x);
 *     — MurmurHash3 fmix64 avalanche of a single u64: xor-shift /
 *       multiply / xor-shift / multiply / xor-shift. Pure; no buffer.
 *   uint64_t __gj_murmur_fmix64  (alias)
 *   __libcgj_batch1442_marker = "libcgj-batch1442"
 *
 * Distinct from gj_hash_mix64 (batch967, same public shape under a
 * different export name), file-static b105_fmix64, and
 * gj_murmur3_32. Unique gj_murmur_fmix64 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * MurmurHash3 fmix64 shape (Appleby).
 */

#include <stdint.h>

const char __libcgj_batch1442_marker[] = "libcgj-batch1442";

/* MurmurHash3 fmix64 constants (public algorithm). */
#define B1442_M1 0xff51afd7ed558ccdULL
#define B1442_M2 0xc4ceb9fe1a85ec53ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_murmur_fmix64 — MurmurHash3-style 64-bit finalizer mix of x.
 *
 * u64X: arbitrary 64-bit input (all values accepted, including 0).
 *
 * Returns the mixed 64-bit output. Stateless pure function.
 */
uint64_t
gj_murmur_fmix64(uint64_t u64X)
{
	uint64_t u64K = u64X;

	u64K ^= u64K >> 33;
	u64K *= B1442_M1;
	u64K ^= u64K >> 33;
	u64K *= B1442_M2;
	u64K ^= u64K >> 33;
	return u64K;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_murmur_fmix64(uint64_t u64X)
    __attribute__((alias("gj_murmur_fmix64")));
