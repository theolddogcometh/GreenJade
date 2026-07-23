/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch966: XXH32 avalanche-only helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_xxh32_avalanche(uint32_t h);
 *     — XXH32 final avalanche mix of a single 32-bit accumulator
 *       (xor-shift / multiply stages). Pure helper; not a buffer hash.
 *   uint32_t gj_xxh32_avalanche(uint32_t h);  (same body, unique short name)
 *   uint32_t __gj_hash_xxh32_avalanche / __gj_xxh32_avalanche  (aliases)
 *   __libcgj_batch966_marker = "libcgj-batch966"
 *
 * Does NOT define gj_xxh32 / __gj_xxh32 (batch458) or XXH32 (batch40) —
 * avalanche helper only. Unique names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party xxHash source copied. Constants match the public
 * XXH32 avalanche shape (PRIME32_2 / PRIME32_3).
 */

#include <stdint.h>

const char __libcgj_batch966_marker[] = "libcgj-batch966";

/* XXH32 primes used by the public avalanche finalizer. */
#define B966_PRIME32_2 0x85EBCA77u
#define B966_PRIME32_3 0xC2B2AE3Du

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_xxh32_avalanche — XXH32 final avalanche of accumulator h.
 *
 * h: pre-length-mixed 32-bit state (or any 32-bit value to scramble)
 *
 * Returns the avalanched 32-bit result. Stateless pure function.
 *
 * Mix (public XXH32 shape):
 *   h ^= h >> 15; h *= PRIME32_2;
 *   h ^= h >> 13; h *= PRIME32_3;
 *   h ^= h >> 16;
 */
uint32_t
gj_hash_xxh32_avalanche(uint32_t h)
{
	uint32_t u32H = h;

	u32H ^= u32H >> 15;
	u32H *= B966_PRIME32_2;
	u32H ^= u32H >> 13;
	u32H *= B966_PRIME32_3;
	u32H ^= u32H >> 16;
	return u32H;
}

/* Unique short name requested as gj_xxh32_avalanche. */
uint32_t gj_xxh32_avalanche(uint32_t h)
    __attribute__((alias("gj_hash_xxh32_avalanche")));

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_hash_xxh32_avalanche(uint32_t h)
    __attribute__((alias("gj_hash_xxh32_avalanche")));

uint32_t __gj_xxh32_avalanche(uint32_t h)
    __attribute__((alias("gj_hash_xxh32_avalanche")));
