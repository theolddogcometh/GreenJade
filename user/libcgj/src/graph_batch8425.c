/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8425: 32-bit Murmur-style finalizer mix (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_mix32_u_8425(uint32_t x);
 *     - MurmurHash3 fmix32 finalizer: xor-shift / multiply / xor-shift /
 *       multiply / xor-shift. Pure; no buffer walk.
 *   uint32_t __gj_hash_mix32_u_8425  (alias)
 *   __libcgj_batch8425_marker = "libcgj-batch8425"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from
 * gj_hash_mix32_u (batch2051) — unique _u_8425 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 * Constants match the public MurmurHash3 fmix32 shape (Appleby).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8425_marker[] = "libcgj-batch8425";

/* MurmurHash3 fmix32 constants (public algorithm). */
#define B8425_M1 0x85ebca6bu
#define B8425_M2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8425_mix32(uint32_t u32X)
{
	uint32_t u32H = u32X;

	u32H ^= u32H >> 16;
	u32H *= B8425_M1;
	u32H ^= u32H >> 13;
	u32H *= B8425_M2;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix32_u_8425 - MurmurHash3-style 32-bit finalizer mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the mixed 32-bit output. Stateless pure function.
 */
uint32_t
gj_hash_mix32_u_8425(uint32_t u32X)
{
	(void)NULL;
	return b8425_mix32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_mix32_u_8425(uint32_t u32X)
    __attribute__((alias("gj_hash_mix32_u_8425")));
