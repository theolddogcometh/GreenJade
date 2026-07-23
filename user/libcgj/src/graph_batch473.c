/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch473: freestanding Spooky-inspired 64-bit hash.
 *
 * Surface (unique symbols):
 *   uint64_t gj_spooky64(const void *data, size_t n, uint64_t seed);
 *     — Simplified SpookyHash-inspired one-shot 64-bit hash of data[0..n)
 *       with 64-bit seed. Not a full SpookyHash128 port; a compact
 *       freestanding integer path using Spooky-style sc_const init,
 *       16-byte block mix, and short-message end scramble.
 *       NULL data with n != 0 → 0; empty n==0 is valid and seed-dependent.
 *   __gj_spooky64  (alias)
 *   __libcgj_batch473_marker = "libcgj-batch473"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch473_marker[] = "libcgj-batch473";

/*
 * SpookyHash public sc_const (Bob Jenkins): odd constant used in Short
 * and full Mix schedules. Used here for a simplified 64-bit digest path.
 */
#define B473_SC 0xdeadbeefdeadbeefull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b473_rotl64(uint64_t uX, unsigned uN)
{
	uN &= 63u;
	if (uN == 0u) {
		return uX;
	}
	return (uX << uN) | (uX >> (64u - uN));
}

/* Little-endian unaligned 64-bit load. */
static uint64_t
b473_read64le(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

/*
 * b473_short_mix — Spooky-inspired short mix of four 64-bit lanes.
 * Rotation schedule adapted from Spooky ShortMix (public description).
 */
static void
b473_short_mix(uint64_t *pH0, uint64_t *pH1, uint64_t *pH2, uint64_t *pH3)
{
	uint64_t h0 = *pH0;
	uint64_t h1 = *pH1;
	uint64_t h2 = *pH2;
	uint64_t h3 = *pH3;

	h2 = b473_rotl64(h2, 50);
	h2 += h3;
	h0 ^= h2;
	h3 = b473_rotl64(h3, 52);
	h3 += h0;
	h1 ^= h3;
	h0 = b473_rotl64(h0, 30);
	h0 += h1;
	h2 ^= h0;
	h1 = b473_rotl64(h1, 41);
	h1 += h2;
	h3 ^= h1;
	h2 = b473_rotl64(h2, 54);
	h2 += h3;
	h0 ^= h2;
	h3 = b473_rotl64(h3, 48);
	h3 += h0;
	h1 ^= h3;
	h0 = b473_rotl64(h0, 38);
	h0 += h1;
	h2 ^= h0;
	h1 = b473_rotl64(h1, 37);
	h1 += h2;
	h3 ^= h1;
	h2 = b473_rotl64(h2, 62);
	h2 += h3;
	h0 ^= h2;
	h3 = b473_rotl64(h3, 34);
	h3 += h0;
	h1 ^= h3;
	h0 = b473_rotl64(h0, 5);
	h0 += h1;
	h2 ^= h0;
	h1 = b473_rotl64(h1, 36);
	h1 += h2;
	h3 ^= h1;

	*pH0 = h0;
	*pH1 = h1;
	*pH2 = h2;
	*pH3 = h3;
}

/*
 * b473_short_end — final avalanche (Spooky ShortEnd-inspired).
 */
static void
b473_short_end(uint64_t *pH0, uint64_t *pH1, uint64_t *pH2, uint64_t *pH3)
{
	uint64_t h0 = *pH0;
	uint64_t h1 = *pH1;
	uint64_t h2 = *pH2;
	uint64_t h3 = *pH3;

	h3 ^= h2;
	h2 = b473_rotl64(h2, 15);
	h3 += h2;
	h0 ^= h3;
	h3 = b473_rotl64(h3, 52);
	h0 += h3;
	h1 ^= h0;
	h0 = b473_rotl64(h0, 26);
	h1 += h0;
	h2 ^= h1;
	h1 = b473_rotl64(h1, 51);
	h2 += h1;
	h3 ^= h2;
	h2 = b473_rotl64(h2, 28);
	h3 += h2;
	h0 ^= h3;
	h3 = b473_rotl64(h3, 9);
	h0 += h3;
	h1 ^= h0;
	h0 = b473_rotl64(h0, 47);
	h1 += h0;
	h2 ^= h1;
	h1 = b473_rotl64(h1, 54);
	h2 += h1;
	h3 ^= h2;
	h2 = b473_rotl64(h2, 32);
	h3 += h2;
	h0 ^= h3;
	h3 = b473_rotl64(h3, 25);
	h0 += h3;
	h1 ^= h0;
	h0 = b473_rotl64(h0, 63);
	h1 += h0;

	*pH0 = h0;
	*pH1 = h1;
	*pH2 = h2;
	*pH3 = h3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spooky64 — simplified Spooky-inspired 64-bit one-shot hash.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 64-bit seed (placed in h0; sc_const + length in remaining lanes).
 *
 * Returns h0 after short-end. NULL data with n != 0 → 0.
 *
 * Sketch:
 *   h0 = seed; h1 = seed; h2 = sc_const; h3 = sc_const
 *   h2 += n
 *   for each full 32-byte block: absorb 4×u64 + short_mix
 *   absorb tail (up to 31 bytes) into the four lanes + short_end
 */
uint64_t
gj_spooky64(const void *data, size_t n, uint64_t seed)
{
	const unsigned char *p;
	const unsigned char *pEnd;
	uint64_t h0;
	uint64_t h1;
	uint64_t h2;
	uint64_t h3;
	size_t cbRem;
	unsigned char aTail[32];
	size_t i;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0ull;
	}

	h0 = seed;
	h1 = seed;
	h2 = B473_SC;
	h3 = B473_SC;
	h2 += (uint64_t)n;

	pEnd = (p == NULL) ? NULL : (p + n);

	/* Full 32-byte blocks. */
	if (p != NULL) {
		while ((size_t)(pEnd - p) >= 32u) {
			h0 += b473_read64le(p);
			h1 += b473_read64le(p + 8);
			h2 += b473_read64le(p + 16);
			h3 += b473_read64le(p + 24);
			b473_short_mix(&h0, &h1, &h2, &h3);
			p += 32;
		}
	}

	/* Zero-pad tail into 32-byte buffer (Spooky short remainder style). */
	for (i = 0u; i < 32u; i++) {
		aTail[i] = 0u;
	}
	cbRem = (p == NULL || pEnd == NULL) ? 0u : (size_t)(pEnd - p);
	for (i = 0u; i < cbRem; i++) {
		aTail[i] = p[i];
	}
	/* Encode remainder length in the last byte (Spooky short convention). */
	aTail[31] = (unsigned char)cbRem;

	h0 += b473_read64le(aTail);
	h1 += b473_read64le(aTail + 8);
	h2 += b473_read64le(aTail + 16);
	h3 += b473_read64le(aTail + 24);
	b473_short_end(&h0, &h1, &h2, &h3);

	return h0;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_spooky64(const void *data, size_t n, uint64_t seed)
    __attribute__((alias("gj_spooky64")));
