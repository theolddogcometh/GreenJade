/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4027: 32-bit avalanche mixer (splitmix32-style).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avalanche_u(uint32_t x);
 *     - Avalanche step: x ^= x>>16; *0x7feb352d; x ^= x>>15;
 *       *0x846ca68b; x ^= x>>16. Diffuses low/high entropy.
 *   uint32_t __gj_u32_avalanche_u  (alias)
 *   __libcgj_batch4027_marker = "libcgj-batch4027"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_u32_avalanche_u surface only; no multi-def.
 * Distinct from gj_u32_mix_final_u (batch4025) — different constants.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4027_marker[] = "libcgj-batch4027";

/* splitmix32-style avalanche constants (public algorithm family). */
#define B4027_C1 0x7feb352du
#define B4027_C2 0x846ca68bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4027_avalanche(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= B4027_C1;
	u32X ^= u32X >> 15;
	u32X *= B4027_C2;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avalanche_u - 32-bit avalanche / bit-diffusion step.
 *
 * x: raw 32-bit value
 *
 * Returns a fully diffused 32-bit value. Pure integer; no side effects.
 * No parent wires.
 */
uint32_t
gj_u32_avalanche_u(uint32_t u32X)
{
	(void)NULL;
	return b4027_avalanche(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avalanche_u(uint32_t u32X)
    __attribute__((alias("gj_u32_avalanche_u")));
