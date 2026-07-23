/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2055: 32-bit degski-style avalanche finalizer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_avalanche32(uint32_t x);
 *     - High-quality 32-bit avalanche: xor-shift / multiply stages with
 *       public odd constants 0x7feb352d / 0x846ca68b. Pure; no buffer.
 *   uint32_t __gj_hash_avalanche32  (alias)
 *   __libcgj_batch2055_marker = "libcgj-batch2055"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_xxh32_avalanche (batch966), gj_hash_mix32_u (batch2051 fmix32),
 * and file-static fmix32 helpers. Unique gj_hash_avalanche32 surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * degski/Lemire 32-bit finalizer shape.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2055_marker[] = "libcgj-batch2055";

/* Public degski-style 32-bit avalanche multipliers. */
#define B2055_M1 0x7feb352du
#define B2055_M2 0x846ca68bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2055_avalanche(uint32_t u32X)
{
	uint32_t u32H = u32X;

	u32H ^= u32H >> 16;
	u32H *= B2055_M1;
	u32H ^= u32H >> 15;
	u32H *= B2055_M2;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_avalanche32 - 32-bit avalanche finalizer of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the avalanched 32-bit result. Stateless pure function.
 */
uint32_t
gj_hash_avalanche32(uint32_t u32X)
{
	(void)NULL;
	return b2055_avalanche(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_avalanche32(uint32_t u32X)
    __attribute__((alias("gj_hash_avalanche32")));
