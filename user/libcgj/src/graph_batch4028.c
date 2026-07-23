/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4028: 64-bit avalanche mixer (splitmix64-style).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avalanche_u(uint64_t x);
 *     - Avalanche step: x ^= x>>30; *0xbf58476d1ce4e5b9;
 *       x ^= x>>27; *0x94d049bb133111eb; x ^= x>>31.
 *       Diffuses low/high entropy across all 64 bits.
 *   uint64_t __gj_u64_avalanche_u  (alias)
 *   __libcgj_batch4028_marker = "libcgj-batch4028"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_u64_avalanche_u surface only; no multi-def.
 * Distinct from gj_u64_mix_final_u (batch4026) — different constants.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4028_marker[] = "libcgj-batch4028";

/* splitmix64-style avalanche constants (public algorithm family). */
#define B4028_C1 0xbf58476d1ce4e5b9ull
#define B4028_C2 0x94d049bb133111ebull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4028_avalanche(uint64_t u64X)
{
	u64X ^= u64X >> 30;
	u64X *= B4028_C1;
	u64X ^= u64X >> 27;
	u64X *= B4028_C2;
	u64X ^= u64X >> 31;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avalanche_u - 64-bit avalanche / bit-diffusion step.
 *
 * x: raw 64-bit value
 *
 * Returns a fully diffused 64-bit value. Pure integer; no side effects.
 * No parent wires.
 */
uint64_t
gj_u64_avalanche_u(uint64_t u64X)
{
	(void)NULL;
	return b4028_avalanche(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avalanche_u(uint64_t u64X)
    __attribute__((alias("gj_u64_avalanche_u")));
