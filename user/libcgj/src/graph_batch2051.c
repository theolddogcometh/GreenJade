/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2051: 32-bit Murmur-style finalizer mix (_u wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_mix32_u(uint32_t x);
 *     - MurmurHash3 fmix32 finalizer: xor-shift / multiply / xor-shift /
 *       multiply / xor-shift. Pure; no buffer walk.
 *   uint32_t __gj_hash_mix32_u  (alias)
 *   __libcgj_batch2051_marker = "libcgj-batch2051"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Name uses _u because
 * mix32 helpers exist file-static elsewhere (e.g. batch105 b105_fmix32);
 * this is the unique exported gj_hash_mix32_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * MurmurHash3 fmix32 shape (Appleby).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2051_marker[] = "libcgj-batch2051";

/* MurmurHash3 fmix32 constants (public algorithm). */
#define B2051_M1 0x85ebca6bu
#define B2051_M2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2051_mix(uint32_t u32X)
{
	uint32_t u32H = u32X;

	u32H ^= u32H >> 16;
	u32H *= B2051_M1;
	u32H ^= u32H >> 13;
	u32H *= B2051_M2;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix32_u - MurmurHash3-style 32-bit finalizer mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns the mixed 32-bit output. Stateless pure function.
 */
uint32_t
gj_hash_mix32_u(uint32_t u32X)
{
	(void)NULL;
	return b2051_mix(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_mix32_u(uint32_t u32X)
    __attribute__((alias("gj_hash_mix32_u")));
