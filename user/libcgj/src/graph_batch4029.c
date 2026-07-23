/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4029: 32-bit key scramble (Murmur-style).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_scramble_u(uint32_t x);
 *     - Key scramble: x *= 0xcc9e2d51; rotl15; x *= 0x1b873593.
 *       Used as a per-word pre-mix before body accumulation.
 *   uint32_t __gj_u32_scramble_u  (alias)
 *   __libcgj_batch4029_marker = "libcgj-batch4029"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Unique gj_u32_scramble_u surface only; no multi-def.
 * Distinct from mix_final / avalanche surfaces — no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4029_marker[] = "libcgj-batch4029";

/* MurmurHash3 x86_32 per-block constants (public algorithm). */
#define B4029_C1 0xcc9e2d51u
#define B4029_C2 0x1b873593u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4029_rotl32(uint32_t u32X, unsigned uR)
{
	return (u32X << uR) | (u32X >> (32u - uR));
}

static uint32_t
b4029_scramble(uint32_t u32X)
{
	u32X *= B4029_C1;
	u32X = b4029_rotl32(u32X, 15u);
	u32X *= B4029_C2;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_scramble_u - 32-bit key scramble (multiply-rotate-multiply).
 *
 * x: raw 32-bit word
 *
 * Returns the scrambled word. Pure integer; no side effects.
 * No parent wires.
 */
uint32_t
gj_u32_scramble_u(uint32_t u32X)
{
	(void)NULL;
	return b4029_scramble(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_scramble_u(uint32_t u32X)
    __attribute__((alias("gj_u32_scramble_u")));
