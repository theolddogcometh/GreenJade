/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4025: final 32-bit mix (MurmurHash3 fmix32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mix_final_u(uint32_t x);
 *     - Avalanche finalizer: x ^= x>>16; *0x85ebca6b; x ^= x>>13;
 *       *0xc2b2ae35; x ^= x>>16. Spreads entropy across all 32 bits.
 *   uint32_t __gj_u32_mix_final_u  (alias)
 *   __libcgj_batch4025_marker = "libcgj-batch4025"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_u32_mix_final_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4025_marker[] = "libcgj-batch4025";

/* MurmurHash3 fmix32 multiplicative constants (public algorithm). */
#define B4025_C1 0x85ebca6bu
#define B4025_C2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4025_fmix32(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= B4025_C1;
	u32X ^= u32X >> 13;
	u32X *= B4025_C2;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mix_final_u - final 32-bit mix / fmix avalanche step.
 *
 * x: raw 32-bit accumulator
 *
 * Returns a well-mixed 32-bit value with improved bit diffusion.
 * Pure integer; no side effects. No parent wires.
 */
uint32_t
gj_u32_mix_final_u(uint32_t u32X)
{
	(void)NULL;
	return b4025_fmix32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mix_final_u(uint32_t u32X)
    __attribute__((alias("gj_u32_mix_final_u")));
