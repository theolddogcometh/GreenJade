/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3184: freestanding NUL-terminated Murmur-style
 * mixed string hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_murmur_mix_u(const char *s);
 *     - Walk the C string at s, mixing each octet with MurmurHash3
 *       x86_32 round constants, then avalanche with fmix32. NULL s -> 0.
 *       Empty string -> fmix32(0) (length-xor already 0).
 *   uint32_t __gj_str_hash_murmur_mix_u  (alias)
 *   __libcgj_batch3184_marker = "libcgj-batch3184"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_str_hash_murmur_mix_u surface only; no multi-def. Distinct from
 * gj_murmur3_32 / murmur3_32 (length-bounded mem hashes) and pure
 * fmix helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match public MurmurHash3
 * x86_32 / fmix32 shape (Appleby).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3184_marker[] = "libcgj-batch3184";

/* MurmurHash3 x86_32 / fmix32 constants (public algorithm). */
#define B3184_C1 0xcc9e2d51u
#define B3184_C2 0x1b873593u
#define B3184_M  5u
#define B3184_N  0xe6546b64u
#define B3184_FM1 0x85ebca6bu
#define B3184_FM2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3184_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b3184_fmix32(uint32_t u32H)
{
	u32H ^= u32H >> 16;
	u32H *= B3184_FM1;
	u32H ^= u32H >> 13;
	u32H *= B3184_FM2;
	u32H ^= u32H >> 16;
	return u32H;
}

/*
 * Per-byte Murmur x86_32 mix over a NUL-terminated string, then fmix32.
 * NULL -> 0. Length is xor'd into the state before finalization.
 */
static uint32_t
b3184_murmur_mix(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;
	uint32_t u32Len;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32H = 0u;
	u32Len = 0u;
	while (*p != 0u) {
		uint32_t u32K = (uint32_t)(*p);

		u32K *= B3184_C1;
		u32K = b3184_rotl32(u32K, 15);
		u32K *= B3184_C2;

		u32H ^= u32K;
		u32H = b3184_rotl32(u32H, 13);
		u32H = u32H * B3184_M + B3184_N;

		p++;
		u32Len++;
	}
	u32H ^= u32Len;
	return b3184_fmix32(u32H);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_murmur_mix_u - Murmur-mixed hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest after per-byte Murmur rounds and fmix32.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_str_hash_murmur_mix_u(const char *sz)
{
	(void)NULL;
	return b3184_murmur_mix(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_murmur_mix_u(const char *sz)
    __attribute__((alias("gj_str_hash_murmur_mix_u")));
