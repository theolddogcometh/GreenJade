/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch457: SuperFastHash 32-bit one-shot
 * (gj-prefixed surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_superfast32(const void *data, size_t n);
 *     — Paul Hsieh SuperFastHash over n bytes at data. Clean-room
 *       integer path (byte-wise LE 16-bit loads). NULL data with
 *       n != 0 → 0. Empty (n == 0) → 0.
 *   __gj_superfast32  (alias)
 *   __libcgj_batch457_marker = "libcgj-batch457"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Algorithm matches the published
 * SuperFastHash public reference (residual bytes use signed char
 * extension as in the classic description).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch457_marker[] = "libcgj-batch457";

/* ---- freestanding helpers ---------------------------------------------- */

/* Little-endian 16-bit load (unaligned-safe). */
static uint32_t
b457_load_le16(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_superfast32 — SuperFastHash 32-bit digest of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns 0.
 */
uint32_t
gj_superfast32(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H;
	uint32_t u32Tmp;
	size_t cBlocks;
	unsigned uRem;
	size_t i;

	if (n == 0u) {
		return 0u;
	}
	if (p == NULL) {
		return 0u;
	}

	/* hash starts as length (truncated to 32 bits for huge n). */
	u32H = (uint32_t)n;
	cBlocks = n >> 2;
	uRem = (unsigned)(n & 3u);

	for (i = 0u; i < cBlocks; i++) {
		u32H += b457_load_le16(p);
		u32Tmp = (b457_load_le16(p + 2) << 11) ^ u32H;
		u32H = (u32H << 16) ^ u32Tmp;
		p += 4;
		u32H += u32H >> 11;
	}

	/* Residual 0..3 bytes (signed residual matching classic SuperFastHash). */
	switch (uRem) {
	case 3:
		u32H += b457_load_le16(p);
		u32H ^= u32H << 16;
		u32H ^= (uint32_t)(int32_t)(int8_t)p[2] << 18;
		u32H += u32H >> 11;
		break;
	case 2:
		u32H += b457_load_le16(p);
		u32H ^= u32H << 11;
		u32H += u32H >> 17;
		break;
	case 1:
		u32H += (uint32_t)(int32_t)(int8_t)p[0];
		u32H ^= u32H << 10;
		u32H += u32H >> 1;
		break;
	default:
		break;
	}

	/* Avalanche final mix. */
	u32H ^= u32H << 3;
	u32H += u32H >> 5;
	u32H ^= u32H << 4;
	u32H += u32H >> 17;
	u32H ^= u32H << 25;
	u32H += u32H >> 6;

	return u32H;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_superfast32(const void *data, size_t n)
    __attribute__((alias("gj_superfast32")));
