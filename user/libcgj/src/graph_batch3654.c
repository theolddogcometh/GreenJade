/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3654: gj_xxh32_final_u - XXH32 stream final.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xxh32_final_u(uint32_t state);
 *     - Finalize a running XXH32-style single-lane accumulator with the
 *       public XXH32 avalanche (xor>>15 / *PRIME32_2 / xor>>13 /
 *       *PRIME32_3 / xor>>16).
 *   uint32_t __gj_xxh32_final_u  (alias)
 *   __libcgj_batch3654_marker = "libcgj-batch3654"
 *
 * Companion to gj_xxh32_update_u (batch3653). Distinct from
 * gj_xxh32_avalanche / gj_hash_xxh32_avalanche (batch966) — unique
 * _final_u name for the streaming pair. Hash exclusive wave (3651-3660).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3654_marker[] = "libcgj-batch3654";

/* XXH32 avalanche primes (public algorithm constants). */
#define B3654_PRIME32_2 0x85EBCA77u
#define B3654_PRIME32_3 0xC2B2AE3Du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3654_avalanche(uint32_t u32H)
{
	u32H ^= u32H >> 15;
	u32H *= B3654_PRIME32_2;
	u32H ^= u32H >> 13;
	u32H *= B3654_PRIME32_3;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xxh32_final_u - finalize a streaming XXH32-style accumulator.
 *
 * state: running state from gj_xxh32_update_u (optionally after length
 *        mix: state += total_len)
 *
 * Returns the avalanched 32-bit digest.
 */
uint32_t
gj_xxh32_final_u(uint32_t u32State)
{
	(void)NULL;
	return b3654_avalanche(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xxh32_final_u(uint32_t u32State)
    __attribute__((alias("gj_xxh32_final_u")));
