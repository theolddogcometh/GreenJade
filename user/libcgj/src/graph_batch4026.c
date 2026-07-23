/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4026: final 64-bit mix (MurmurHash3 fmix64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mix_final_u(uint64_t x);
 *     - Avalanche finalizer: x ^= x>>33; *0xff51afd7ed558ccd;
 *       x ^= x>>33; *0xc4ceb9fe1a85ec53; x ^= x>>33.
 *       Spreads entropy across all 64 bits.
 *   uint64_t __gj_u64_mix_final_u  (alias)
 *   __libcgj_batch4026_marker = "libcgj-batch4026"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_u64_mix_final_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4026_marker[] = "libcgj-batch4026";

/* MurmurHash3 fmix64 multiplicative constants (public algorithm). */
#define B4026_C1 0xff51afd7ed558ccdull
#define B4026_C2 0xc4ceb9fe1a85ec53ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4026_fmix64(uint64_t u64X)
{
	u64X ^= u64X >> 33;
	u64X *= B4026_C1;
	u64X ^= u64X >> 33;
	u64X *= B4026_C2;
	u64X ^= u64X >> 33;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mix_final_u - final 64-bit mix / fmix avalanche step.
 *
 * x: raw 64-bit accumulator
 *
 * Returns a well-mixed 64-bit value with improved bit diffusion.
 * Pure integer; no side effects. No parent wires.
 */
uint64_t
gj_u64_mix_final_u(uint64_t u64X)
{
	(void)NULL;
	return b4026_fmix64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mix_final_u(uint64_t u64X)
    __attribute__((alias("gj_u64_mix_final_u")));
