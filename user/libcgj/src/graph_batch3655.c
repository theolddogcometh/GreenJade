/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3655: gj_siphash_half_u - HalfSipHash mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_siphash_half_u(uint32_t x);
 *     - HalfSipHash-inspired pure mix of a single u32: initialize the
 *       four 32-bit HalfSipHash lanes from x, run two compression SIPROUND
 *       steps and four finalization SIPROUND steps, return v1 ^ v3.
 *       Pure; no buffer walk.
 *   uint32_t __gj_siphash_half_u  (alias)
 *   __libcgj_batch3655_marker = "libcgj-batch3655"
 *
 * Hash exclusive wave (3651-3660). Distinct from halfsiphash24 (batch75),
 * gj_siphash24 (batch265), gj_siphash13_64 (batch623) — unique single-word
 * HalfSipHash-style _half_u mixer only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3655_marker[] = "libcgj-batch3655";

/* HalfSipHash public init constants (Aumasson / Bernstein family). */
#define B3655_V0 0x6c796765u
#define B3655_V1 0x74656462u
#define B3655_V2 0x6f6d6573u
#define B3655_V3 0x79746573u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3655_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

/*
 * One HalfSipHash SIPROUND on four 32-bit lanes (in/out via pointers).
 * Public rotation distances: 5, 8, 7, 13, 16 (HalfSipHash-2-4).
 */
static void
b3655_sipround(uint32_t *pV0, uint32_t *pV1, uint32_t *pV2, uint32_t *pV3)
{
	uint32_t v0 = *pV0;
	uint32_t v1 = *pV1;
	uint32_t v2 = *pV2;
	uint32_t v3 = *pV3;

	v0 += v1;
	v1 = b3655_rotl32(v1, 5);
	v1 ^= v0;
	v0 = b3655_rotl32(v0, 16);
	v2 += v3;
	v3 = b3655_rotl32(v3, 8);
	v3 ^= v2;
	v0 += v3;
	v3 = b3655_rotl32(v3, 7);
	v3 ^= v0;
	v2 += v1;
	v1 = b3655_rotl32(v1, 13);
	v1 ^= v2;
	v2 = b3655_rotl32(v2, 16);

	*pV0 = v0;
	*pV1 = v1;
	*pV2 = v2;
	*pV3 = v3;
}

static uint32_t
b3655_half_mix(uint32_t u32X)
{
	uint32_t v0 = B3655_V0 ^ u32X;
	uint32_t v1 = B3655_V1 ^ u32X;
	uint32_t v2 = B3655_V2 ^ u32X;
	uint32_t v3 = B3655_V3 ^ u32X;
	unsigned uR;

	/* Absorb x as a single 32-bit message word (c = 2). */
	v3 ^= u32X;
	for (uR = 0u; uR < 2u; uR++) {
		b3655_sipround(&v0, &v1, &v2, &v3);
	}
	v0 ^= u32X;

	/* Finalization (d = 4) with HalfSipHash end-of-message bit. */
	v2 ^= 0xffu;
	for (uR = 0u; uR < 4u; uR++) {
		b3655_sipround(&v0, &v1, &v2, &v3);
	}
	return v1 ^ v3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_siphash_half_u - HalfSipHash-style pure mix of a single u32.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns a well-diffused 32-bit tag. Stateless pure function.
 */
uint32_t
gj_siphash_half_u(uint32_t u32X)
{
	(void)NULL;
	return b3655_half_mix(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_siphash_half_u(uint32_t u32X)
    __attribute__((alias("gj_siphash_half_u")));
