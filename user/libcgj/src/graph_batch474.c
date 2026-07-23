/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch474: freestanding MurmurHash2 32-bit one-shot.
 *
 * Surface (unique symbols):
 *   uint32_t gj_murmur2_32(const void *data, size_t n, uint32_t seed);
 *     — Austin Appleby MurmurHash2 (32-bit) over data[0..n) with seed.
 *       Public mix: m = 0x5bd1e995, r = 24; h = seed ^ n.
 *       NULL data with n != 0 → 0; empty n==0 is valid and seed-dependent.
 *   __gj_murmur2_32  (alias)
 *   __libcgj_batch474_marker = "libcgj-batch474"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch474_marker[] = "libcgj-batch474";

/* MurmurHash2 public constants. */
#define B474_M 0x5bd1e995u
#define B474_R 24u

/* ---- freestanding helpers ---------------------------------------------- */

/* Little-endian unaligned 32-bit load. */
static uint32_t
b474_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_murmur2_32 — MurmurHash2 32-bit one-shot.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count (folded into initial h as seed ^ n).
 * seed: 32-bit seed.
 *
 * Returns the 32-bit digest. NULL data with n != 0 → 0.
 *
 * Algorithm (MurmurHash2):
 *   h = seed ^ n
 *   for each full 4-byte little-endian word k:
 *     k *= m; k ^= k >> r; k *= m; h *= m; h ^= k
 *   mix remaining 1..3 bytes into h
 *   h ^= h >> 13; h *= m; h ^= h >> 15
 */
uint32_t
gj_murmur2_32(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p;
	uint32_t uH;
	size_t cbLeft;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0u;
	}

	uH = seed ^ (uint32_t)n;
	cbLeft = n;

	while (cbLeft >= 4u) {
		uint32_t uK;

		uK = b474_read32le(p);
		uK *= B474_M;
		uK ^= uK >> B474_R;
		uK *= B474_M;

		uH *= B474_M;
		uH ^= uK;

		p += 4;
		cbLeft -= 4u;
	}

	/* Tail 1..3 bytes (little-endian partial word). */
	switch (cbLeft) {
	case 3u:
		uH ^= ((uint32_t)p[2] << 16);
		/* fallthrough */
	case 2u:
		uH ^= ((uint32_t)p[1] << 8);
		/* fallthrough */
	case 1u:
		uH ^= (uint32_t)p[0];
		uH *= B474_M;
		break;
	default:
		break;
	}

	/* Final avalanche. */
	uH ^= uH >> 13;
	uH *= B474_M;
	uH ^= uH >> 15;
	return uH;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_murmur2_32(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_murmur2_32")));
