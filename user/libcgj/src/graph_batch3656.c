/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3656: gj_murmur3_mix_u - MurmurHash3 fmix32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_murmur3_mix_u(uint32_t x);
 *     - MurmurHash3 x86_32 finalizer (fmix32): xor-shift / multiply /
 *       xor-shift / multiply / xor-shift with public constants
 *       0x85ebca6b / 0xc2b2ae35. Pure; no buffer walk.
 *   uint32_t __gj_murmur3_mix_u  (alias)
 *   __libcgj_batch3656_marker = "libcgj-batch3656"
 *
 * Hash exclusive wave (3651-3660). Distinct from gj_hash_mix32_u
 * (batch2051, same public shape under a different name), gj_murmur3_32
 * (batch291 one-shot), and gj_murmur_fmix64 (batch1442). Unique
 * gj_murmur3_mix_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3656_marker[] = "libcgj-batch3656";

/* MurmurHash3 fmix32 constants (public algorithm). */
#define B3656_M1 0x85ebca6bu
#define B3656_M2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3656_fmix32(uint32_t u32H)
{
	u32H ^= u32H >> 16;
	u32H *= B3656_M1;
	u32H ^= u32H >> 13;
	u32H *= B3656_M2;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_murmur3_mix_u - MurmurHash3-style 32-bit finalizer mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the mixed 32-bit output. Stateless pure function.
 */
uint32_t
gj_murmur3_mix_u(uint32_t u32X)
{
	(void)NULL;
	return b3656_fmix32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_murmur3_mix_u(uint32_t u32X)
    __attribute__((alias("gj_murmur3_mix_u")));
