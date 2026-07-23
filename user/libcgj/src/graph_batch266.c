/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch266: XXH64 one-shot under a unique gj_ name.
 *
 * Surface (unique symbols only — do not export XXH64 / __XXH64):
 *   uint64_t gj_xxh64(const void *data, size_t n, uint64_t seed);
 *     — xxHash XXH64 one-shot over data[0..n). Clean-room integer path.
 *       data may be NULL only when n == 0; otherwise NULL+n>0 → 0.
 *   __gj_xxh64  (alias)
 *   __libcgj_batch266_marker = "libcgj-batch266"
 *
 * Avoid multi-def with graph_batch40.c XXH64/__XXH64: only gj_xxh64 is
 * exported from this TU. Clean-room freestanding pure C (integer/pointer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno. No third-party xxHash source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch266_marker[] = "libcgj-batch266";

/* XXH64 primes (public algorithm constants). */
#define B266_PRIME64_1 0x9E3779B185EBCA87ull
#define B266_PRIME64_2 0xC2B2AE3D27D4EB4Full
#define B266_PRIME64_3 0x165667B19E3779F9ull
#define B266_PRIME64_4 0x85EBCA77C2B2AE63ull
#define B266_PRIME64_5 0x27D4EB2F165667C5ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b266_rotl64(uint64_t u64X, unsigned uN)
{
	return (u64X << uN) | (u64X >> (64u - uN));
}

static uint32_t
b266_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static uint64_t
b266_read64le(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) | ((uint64_t)p[2] << 16) |
	       ((uint64_t)p[3] << 24) | ((uint64_t)p[4] << 32) |
	       ((uint64_t)p[5] << 40) | ((uint64_t)p[6] << 48) |
	       ((uint64_t)p[7] << 56);
}

static uint64_t
b266_xxh64_round(uint64_t u64Acc, uint64_t u64Input)
{
	u64Acc += u64Input * B266_PRIME64_2;
	u64Acc = b266_rotl64(u64Acc, 31);
	u64Acc *= B266_PRIME64_1;
	return u64Acc;
}

static uint64_t
b266_xxh64_merge(uint64_t u64Acc, uint64_t u64Val)
{
	u64Val = b266_xxh64_round(0, u64Val);
	u64Acc ^= u64Val;
	u64Acc = u64Acc * B266_PRIME64_1 + B266_PRIME64_4;
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xxh64 — XXH64 one-shot hash.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 64-bit seed (public xxHash parameter).
 *
 * Returns the 64-bit digest. NULL data with n != 0 yields 0 (defensive).
 * Empty input (n == 0) is valid and seed-dependent (e.g. seed 0 →
 * 0xEF46DB3751D8E999).
 */
uint64_t
gj_xxh64(const void *data, size_t n, uint64_t seed)
{
	const unsigned char *p = (const unsigned char *)data;
	uint64_t u64H;

	if (p == NULL && n != 0u) {
		return 0ull;
	}
	if (n >= 32u) {
		const unsigned char *pLimit = p + n - 32u;
		uint64_t u64V1 = seed + B266_PRIME64_1 + B266_PRIME64_2;
		uint64_t u64V2 = seed + B266_PRIME64_2;
		uint64_t u64V3 = seed + 0ull;
		uint64_t u64V4 = seed - B266_PRIME64_1;

		do {
			u64V1 = b266_xxh64_round(u64V1, b266_read64le(p));
			p += 8;
			u64V2 = b266_xxh64_round(u64V2, b266_read64le(p));
			p += 8;
			u64V3 = b266_xxh64_round(u64V3, b266_read64le(p));
			p += 8;
			u64V4 = b266_xxh64_round(u64V4, b266_read64le(p));
			p += 8;
		} while (p <= pLimit);
		u64H = b266_rotl64(u64V1, 1) + b266_rotl64(u64V2, 7) +
		       b266_rotl64(u64V3, 12) + b266_rotl64(u64V4, 18);
		u64H = b266_xxh64_merge(u64H, u64V1);
		u64H = b266_xxh64_merge(u64H, u64V2);
		u64H = b266_xxh64_merge(u64H, u64V3);
		u64H = b266_xxh64_merge(u64H, u64V4);
	} else {
		u64H = seed + B266_PRIME64_5;
	}
	u64H += (uint64_t)n;
	{
		const unsigned char *pEnd =
		    (const unsigned char *)data + n;

		while (p + 8u <= pEnd) {
			uint64_t u64K1 = b266_xxh64_round(0, b266_read64le(p));

			u64H ^= u64K1;
			u64H = b266_rotl64(u64H, 27) * B266_PRIME64_1 +
			       B266_PRIME64_4;
			p += 8;
		}
		if (p + 4u <= pEnd) {
			u64H ^= (uint64_t)b266_read32le(p) * B266_PRIME64_1;
			u64H = b266_rotl64(u64H, 23) * B266_PRIME64_2 +
			       B266_PRIME64_3;
			p += 4;
		}
		while (p < pEnd) {
			u64H ^= (uint64_t)(*p) * B266_PRIME64_5;
			u64H = b266_rotl64(u64H, 11) * B266_PRIME64_1;
			p++;
		}
	}
	u64H ^= u64H >> 33;
	u64H *= B266_PRIME64_2;
	u64H ^= u64H >> 29;
	u64H *= B266_PRIME64_3;
	u64H ^= u64H >> 32;
	return u64H;
}

uint64_t __gj_xxh64(const void *data, size_t n, uint64_t seed)
    __attribute__((alias("gj_xxh64")));
