/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch472: freestanding Jenkins lookup3 one-word hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lookup3(const void *data, size_t n, uint32_t seed);
 *     — Bob Jenkins lookup3 / hashlittle-style one-word (32-bit) hash of
 *       data[0..n) with 32-bit seed. Little-endian byte absorption.
 *       NULL data with n != 0 → 0; empty n==0 is valid and seed-dependent.
 *   __gj_lookup3  (alias)
 *   __libcgj_batch472_marker = "libcgj-batch472"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch472_marker[] = "libcgj-batch472";

/* lookup3 golden ratio init constant (public Jenkins schedule). */
#define B472_INIT 0xdeadbeefu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b472_rotl32(uint32_t uX, unsigned uN)
{
	uN &= 31u;
	if (uN == 0u) {
		return uX;
	}
	return (uX << uN) | (uX >> (32u - uN));
}

/*
 * b472_mix — Jenkins lookup3 mix of three 32-bit words (public schedule).
 */
static void
b472_mix(uint32_t *pA, uint32_t *pB, uint32_t *pC)
{
	uint32_t a = *pA;
	uint32_t b = *pB;
	uint32_t c = *pC;

	a -= c;
	a ^= b472_rotl32(c, 4);
	c += b;
	b -= a;
	b ^= b472_rotl32(a, 6);
	a += c;
	c -= b;
	c ^= b472_rotl32(b, 8);
	b += a;
	a -= c;
	a ^= b472_rotl32(c, 16);
	c += b;
	b -= a;
	b ^= b472_rotl32(a, 19);
	a += c;
	c -= b;
	c ^= b472_rotl32(b, 4);
	b += a;

	*pA = a;
	*pB = b;
	*pC = c;
}

/*
 * b472_final — Jenkins lookup3 final mix; c is the one-word digest.
 */
static void
b472_final(uint32_t *pA, uint32_t *pB, uint32_t *pC)
{
	uint32_t a = *pA;
	uint32_t b = *pB;
	uint32_t c = *pC;

	c ^= b;
	c -= b472_rotl32(b, 14);
	a ^= c;
	a -= b472_rotl32(c, 11);
	b ^= a;
	b -= b472_rotl32(a, 25);
	c ^= b;
	c -= b472_rotl32(b, 16);
	a ^= c;
	a -= b472_rotl32(c, 4);
	b ^= a;
	b -= b472_rotl32(a, 14);
	c ^= b;
	c -= b472_rotl32(b, 24);

	*pA = a;
	*pB = b;
	*pC = c;
}

/* Little-endian unaligned 32-bit load (safe freestanding path). */
static uint32_t
b472_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lookup3 — Jenkins lookup3 one-word (32-bit) hash.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 32-bit seed mixed into initial c (and b).
 *
 * Returns the 32-bit digest (final c). NULL data with n != 0 → 0.
 * Empty input is valid: returns final(mix of init state with length).
 *
 * Schedule (hashlittle-style, one-word result):
 *   a = b = c = 0xdeadbeef + (uint32_t)n + seed
 *   absorb 12-byte blocks with mix; tail + final.
 */
uint32_t
gj_lookup3(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p;
	uint32_t a;
	uint32_t b;
	uint32_t c;
	size_t cbLeft;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0u;
	}

	/* Length folded into all three words; seed into b and c. */
	a = B472_INIT + (uint32_t)n + seed;
	b = B472_INIT + (uint32_t)n + seed;
	c = B472_INIT + (uint32_t)n + seed;

	cbLeft = n;
	while (cbLeft > 12u) {
		a += b472_read32le(p);
		b += b472_read32le(p + 4);
		c += b472_read32le(p + 8);
		b472_mix(&a, &b, &c);
		p += 12;
		cbLeft -= 12u;
	}

	/* Tail: fall-through switch on remaining 0..12 bytes. */
	switch (cbLeft) {
	case 12u:
		c += ((uint32_t)p[11] << 24);
		/* fallthrough */
	case 11u:
		c += ((uint32_t)p[10] << 16);
		/* fallthrough */
	case 10u:
		c += ((uint32_t)p[9] << 8);
		/* fallthrough */
	case 9u:
		c += (uint32_t)p[8];
		/* fallthrough */
	case 8u:
		b += ((uint32_t)p[7] << 24);
		/* fallthrough */
	case 7u:
		b += ((uint32_t)p[6] << 16);
		/* fallthrough */
	case 6u:
		b += ((uint32_t)p[5] << 8);
		/* fallthrough */
	case 5u:
		b += (uint32_t)p[4];
		/* fallthrough */
	case 4u:
		a += ((uint32_t)p[3] << 24);
		/* fallthrough */
	case 3u:
		a += ((uint32_t)p[2] << 16);
		/* fallthrough */
	case 2u:
		a += ((uint32_t)p[1] << 8);
		/* fallthrough */
	case 1u:
		a += (uint32_t)p[0];
		b472_final(&a, &b, &c);
		break;
	case 0u:
		/*
		 * Empty or exact multiple of 12: Jenkins hashlittle returns
		 * c with no extra final() (zero-length needs no mixing).
		 */
		break;
	default:
		break;
	}

	return c;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lookup3(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_lookup3")));
