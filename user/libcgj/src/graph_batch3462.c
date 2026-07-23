/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3462: freestanding NUL-terminated XXH32-style
 * mixed string hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_xx_mix_u(const char *s);
 *     - Walk the C string at s, mixing each octet with XXH32 per-byte
 *       primes (PRIME32_5 / rotl11 / PRIME32_1), then apply the XXH32
 *       avalanche. Seed path matches empty XXH32(s="", seed=0): start
 *       at PRIME32_5, add length, mix bytes, avalanche. NULL s -> 0.
 *   uint32_t __gj_str_hash_xx_mix_u  (alias)
 *   __libcgj_batch3462_marker = "libcgj-batch3462"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Unique
 * gj_str_hash_xx_mix_u surface only; no multi-def. Distinct from XXH32
 * (batch40, length-bounded mem hash) and gj_str_hash_murmur_mix_u
 * (batch3184). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match public XXH32
 * primes / avalanche (Collet).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3462_marker[] = "libcgj-batch3462";

/* XXH32 primes / avalanche constants (public algorithm). */
#define B3462_PRIME32_1 0x9E3779B1u
#define B3462_PRIME32_2 0x85EBCA77u
#define B3462_PRIME32_3 0xC2B2AE3Du
#define B3462_PRIME32_5 0x165667B1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3462_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b3462_xx_avalanche(uint32_t u32H)
{
	u32H ^= u32H >> 15;
	u32H *= B3462_PRIME32_2;
	u32H ^= u32H >> 13;
	u32H *= B3462_PRIME32_3;
	u32H ^= u32H >> 16;
	return u32H;
}

/*
 * Per-byte XXH32 mix over a NUL-terminated string, then avalanche.
 * NULL -> 0. Length is added into the state before the byte loop
 * (XXH32 seed=0 short-path shape).
 */
static uint32_t
b3462_xx_mix(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;
	uint32_t u32Len;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32Len = 0u;
	while (p[u32Len] != 0u) {
		u32Len++;
	}
	/* seed=0 short path: h = PRIME32_5 + len, then per-byte mix */
	u32H = B3462_PRIME32_5 + u32Len;
	while (*p != 0u) {
		u32H += (uint32_t)(*p) * B3462_PRIME32_5;
		u32H = b3462_rotl32(u32H, 11) * B3462_PRIME32_1;
		p++;
	}
	return b3462_xx_avalanche(u32H);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_xx_mix_u - XXH32-style mixed hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest after per-byte XXH32 rounds and avalanche.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_str_hash_xx_mix_u(const char *sz)
{
	(void)NULL;
	return b3462_xx_mix(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_xx_mix_u(const char *sz)
    __attribute__((alias("gj_str_hash_xx_mix_u")));
