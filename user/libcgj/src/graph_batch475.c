/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch475: freestanding FastHash64 one-shot.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fasthash64(const void *data, size_t n, uint64_t seed);
 *     — Zilong Tan FastHash64 over data[0..n) with 64-bit seed.
 *       Public m = 0x880355f21e6d1965; mix via (h ^ (h >> 23)) * m.
 *       NULL data with n != 0 → 0; empty n==0 is valid and seed-dependent.
 *   __gj_fasthash64  (alias)
 *   __libcgj_batch475_marker = "libcgj-batch475"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch475_marker[] = "libcgj-batch475";

/* FastHash public mix multiplier. */
#define B475_M 0x880355f21e6d1965ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b475_mix(uint64_t uH)
{
	uH ^= uH >> 23;
	uH *= B475_M;
	uH ^= uH >> 47;
	return uH;
}

/* Little-endian unaligned 64-bit load. */
static uint64_t
b475_read64le(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fasthash64 — FastHash64 one-shot.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count (added into initial h as seed + n * m).
 * seed: 64-bit seed.
 *
 * Returns the 64-bit digest. NULL data with n != 0 → 0.
 *
 * Algorithm (FastHash64):
 *   h = seed ^ (n * m)
 *   for each full 8-byte LE word v: h ^= mix(v); h *= m
 *   pack remaining 1..7 bytes into v; h ^= mix(v); h *= m  (if any)
 *   return mix(h)
 */
uint64_t
gj_fasthash64(const void *data, size_t n, uint64_t seed)
{
	const unsigned char *p;
	uint64_t uH;
	size_t cbLeft;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0ull;
	}

	uH = seed ^ ((uint64_t)n * B475_M);
	cbLeft = n;

	while (cbLeft >= 8u) {
		uint64_t uV;

		uV = b475_read64le(p);
		uH ^= b475_mix(uV);
		uH *= B475_M;
		p += 8;
		cbLeft -= 8u;
	}

	if (cbLeft != 0u) {
		uint64_t uV;
		size_t i;

		uV = 0ull;
		for (i = 0u; i < cbLeft; i++) {
			uV |= ((uint64_t)p[i] << (i * 8u));
		}
		uH ^= b475_mix(uV);
		uH *= B475_M;
	}

	return b475_mix(uH);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fasthash64(const void *data, size_t n, uint64_t seed)
    __attribute__((alias("gj_fasthash64")));
