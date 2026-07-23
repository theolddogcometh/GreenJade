/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch460: freestanding MetroHash64 one-shot.
 *
 * Surface (unique symbols):
 *   uint64_t gj_metro64(const void *data, size_t n, uint64_t seed);
 *     — MetroHash64 one-shot over data[0..n) with 64-bit seed.
 *       Clean-room integer path of the public MetroHash64::Hash
 *       schedule (k0..k3 = 0xD6D018F5 / 0xA2AA033B / 0x62992FC1 /
 *       0x30BC5B29). NULL data with n != 0 → 0; empty n==0 is valid
 *       and seed-dependent.
 *   __gj_metro64  (alias)
 *   __libcgj_batch460_marker = "libcgj-batch460"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch460_marker[] = "libcgj-batch460";

/* MetroHash64 public mix constants (Rogers MetroHash64 class schedule). */
#define B460_K0 0xD6D018F5ull
#define B460_K1 0xA2AA033Bull
#define B460_K2 0x62992FC1ull
#define B460_K3 0x30BC5B29ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b460_rotr64(uint64_t u64X, unsigned uN)
{
	uN &= 63u;
	if (uN == 0u) {
		return u64X;
	}
	return (u64X >> uN) | (u64X << (64u - uN));
}

/* Little-endian unaligned 16-bit load. */
static uint16_t
b460_read16le(const unsigned char *p)
{
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

/* Little-endian unaligned 32-bit load. */
static uint32_t
b460_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Little-endian unaligned 64-bit load. */
static uint64_t
b460_read64le(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_metro64 — MetroHash64 one-shot hash.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 64-bit seed mixed into the initial state.
 *
 * Returns the 64-bit digest. NULL data with n != 0 yields 0 (defensive).
 * Empty input (n == 0) is valid and seed-dependent.
 *
 * Algorithm sketch (MetroHash64::Hash):
 *   h = (seed + K2) * K0
 *   if n >= 32: 4-lane 32-byte bulk + cross-mix fold into h
 *   tail 16 / 8 / 4 / 2 / 1 with rotate-xor avalanche
 *   final: h ^= rotr(h,28); h *= K0; h ^= rotr(h,29)
 */
uint64_t
gj_metro64(const void *data, size_t n, uint64_t seed)
{
	const unsigned char *p;
	const unsigned char *pEnd;
	uint64_t u64H;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0ull;
	}

	pEnd = p + n;
	u64H = (seed + B460_K2) * B460_K0;

	if (n >= 32u) {
		uint64_t u64V0 = u64H;
		uint64_t u64V1 = u64H;
		uint64_t u64V2 = u64H;
		uint64_t u64V3 = u64H;

		do {
			u64V0 += b460_read64le(p) * B460_K0;
			p += 8;
			u64V0 = b460_rotr64(u64V0, 29) + u64V2;

			u64V1 += b460_read64le(p) * B460_K1;
			p += 8;
			u64V1 = b460_rotr64(u64V1, 29) + u64V3;

			u64V2 += b460_read64le(p) * B460_K2;
			p += 8;
			u64V2 = b460_rotr64(u64V2, 29) + u64V0;

			u64V3 += b460_read64le(p) * B460_K3;
			p += 8;
			u64V3 = b460_rotr64(u64V3, 29) + u64V1;
		} while (p <= (pEnd - 32));

		u64V2 ^= b460_rotr64(((u64V0 + u64V3) * B460_K0) + u64V1, 37) *
		         B460_K1;
		u64V3 ^= b460_rotr64(((u64V1 + u64V2) * B460_K1) + u64V0, 37) *
		         B460_K0;
		u64V0 ^= b460_rotr64(((u64V0 + u64V2) * B460_K0) + u64V3, 37) *
		         B460_K1;
		u64V1 ^= b460_rotr64(((u64V1 + u64V3) * B460_K1) + u64V2, 37) *
		         B460_K0;
		u64H += u64V0 ^ u64V1;
	}

	if ((size_t)(pEnd - p) >= 16u) {
		uint64_t u64V0;
		uint64_t u64V1;

		u64V0 = u64H + (b460_read64le(p) * B460_K2);
		p += 8;
		u64V0 = b460_rotr64(u64V0, 29) * B460_K3;

		u64V1 = u64H + (b460_read64le(p) * B460_K2);
		p += 8;
		u64V1 = b460_rotr64(u64V1, 29) * B460_K3;

		u64V0 ^= b460_rotr64(u64V0 * B460_K0, 21) + u64V1;
		u64V1 ^= b460_rotr64(u64V1 * B460_K3, 21) + u64V0;
		u64H += u64V1;
	}

	if ((size_t)(pEnd - p) >= 8u) {
		u64H += b460_read64le(p) * B460_K3;
		p += 8;
		u64H ^= b460_rotr64(u64H, 55) * B460_K1;
	}

	if ((size_t)(pEnd - p) >= 4u) {
		u64H += (uint64_t)b460_read32le(p) * B460_K3;
		p += 4;
		u64H ^= b460_rotr64(u64H, 26) * B460_K1;
	}

	if ((size_t)(pEnd - p) >= 2u) {
		u64H += (uint64_t)b460_read16le(p) * B460_K3;
		p += 2;
		u64H ^= b460_rotr64(u64H, 48) * B460_K1;
	}

	if ((size_t)(pEnd - p) >= 1u) {
		u64H += (uint64_t)(*p) * B460_K3;
		u64H ^= b460_rotr64(u64H, 37) * B460_K1;
	}

	u64H ^= b460_rotr64(u64H, 28);
	u64H *= B460_K0;
	u64H ^= b460_rotr64(u64H, 29);
	return u64H;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_metro64(const void *data, size_t n, uint64_t seed)
    __attribute__((alias("gj_metro64")));
