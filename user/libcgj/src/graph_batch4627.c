/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4627: 64-bit entropy avalanche (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_entropy_avalanche64_u(uint64_t x);
 *     - Strong avalanche finalizer for entropy pipelines:
 *       xorshift / odd-multiply repeated for full bit diffusion.
 *   uint64_t __gj_entropy_avalanche64_u  (alias)
 *   __libcgj_batch4627_marker = "libcgj-batch4627"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_avalanche64_u surface only; no multi-def. Distinct from
 * gj_u64_avalanche_u (batch4028), gj_u64_mix_final_u (batch4026),
 * gj_entropy_mix64_u (batch4622) — different constants and shift
 * schedule. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4627_marker[] = "libcgj-batch4627";

/* Avalanche constants unique to this TU (not splitmix/murmur/moremur). */
#define B4627_C1  0xe7037ed1a0b428dbULL
#define B4627_C2  0x8ebc6af645a1c6d7ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4627_avalanche64(uint64_t u64X)
{
	u64X ^= u64X >> 28;
	u64X *= B4627_C1;
	u64X ^= u64X >> 27;
	u64X *= B4627_C2;
	u64X ^= u64X >> 31;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_avalanche64_u - 64-bit entropy avalanche / bit diffusion.
 *
 * x: raw 64-bit accumulator
 *
 * Returns a fully diffused 64-bit value. Pure integer; no side effects.
 * No parent wires.
 */
uint64_t
gj_entropy_avalanche64_u(uint64_t u64X)
{
	(void)NULL;
	return b4627_avalanche64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_entropy_avalanche64_u(uint64_t u64X)
    __attribute__((alias("gj_entropy_avalanche64_u")));
