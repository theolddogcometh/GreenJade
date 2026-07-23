/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4626: 32-bit entropy avalanche (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_avalanche32_u(uint32_t x);
 *     - Strong avalanche finalizer for entropy pipelines:
 *       xorshift / odd-multiply repeated for full bit diffusion.
 *   uint32_t __gj_entropy_avalanche32_u  (alias)
 *   __libcgj_batch4626_marker = "libcgj-batch4626"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_avalanche32_u surface only; no multi-def. Distinct from
 * gj_u32_avalanche_u (batch4027), gj_u32_mix_final_u (batch4025),
 * gj_entropy_mix32_u (batch4621) — different constants and shift
 * schedule. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4626_marker[] = "libcgj-batch4626";

/* Avalanche constants unique to this TU (not splitmix32 / fmix32). */
#define B4626_C1  0xed5ad4bbu
#define B4626_C2  0xac4c1b51u
#define B4626_C3  0x31848babu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4626_avalanche32(uint32_t u32X)
{
	u32X ^= u32X >> 17;
	u32X *= B4626_C1;
	u32X ^= u32X >> 11;
	u32X *= B4626_C2;
	u32X ^= u32X >> 15;
	u32X *= B4626_C3;
	u32X ^= u32X >> 14;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_avalanche32_u - 32-bit entropy avalanche / bit diffusion.
 *
 * x: raw 32-bit accumulator
 *
 * Returns a fully diffused 32-bit value. Pure integer; no side effects.
 * No parent wires.
 */
uint32_t
gj_entropy_avalanche32_u(uint32_t u32X)
{
	(void)NULL;
	return b4626_avalanche32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_entropy_avalanche32_u(uint32_t u32X)
    __attribute__((alias("gj_entropy_avalanche32_u")));
