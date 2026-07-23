/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch105: MurmurHash3 full Appleby surface.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Userspace may be built with -msse2; this TU does not require SSE.
 *
 * batch41 already exports murmur3_32 (size_t length). This TU is unique:
 *   uint32_t MurmurHash3_x86_32(const void *key, int len, uint32_t seed);
 *   void MurmurHash3_x64_128(const void *key, const int len,
 *                            uint32_t seed, void *out);
 *   __MurmurHash3_x86_32 / __MurmurHash3_x64_128  (aliases)
 *   __libcgj_batch105_marker = "libcgj-batch105"
 *
 * Algorithm: Austin Appleby MurmurHash3 (public domain). No third-party
 * source tree was copied; constants and round structure match the
 * published SMHasher reference.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch105_marker[] = "libcgj-batch105";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b105_rotl32(uint32_t x, unsigned r)
{
	return (x << r) | (x >> (32u - r));
}

static uint64_t
b105_rotl64(uint64_t x, unsigned r)
{
	return (x << r) | (x >> (64u - r));
}

static uint32_t
b105_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t
b105_load_le64(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

static void
b105_store_le64(unsigned char *p, uint64_t x)
{
	p[0] = (unsigned char)x;
	p[1] = (unsigned char)(x >> 8);
	p[2] = (unsigned char)(x >> 16);
	p[3] = (unsigned char)(x >> 24);
	p[4] = (unsigned char)(x >> 32);
	p[5] = (unsigned char)(x >> 40);
	p[6] = (unsigned char)(x >> 48);
	p[7] = (unsigned char)(x >> 56);
}

static uint32_t
b105_fmix32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6bu;
	h ^= h >> 13;
	h *= 0xc2b2ae35u;
	h ^= h >> 16;
	return h;
}

static uint64_t
b105_fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdull;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53ull;
	k ^= k >> 33;
	return k;
}

/* ---- MurmurHash3 x86_32 ------------------------------------------------ */

/*
 * Returns the 32-bit hash (Appleby out-parameter form folded to a return).
 * len < 0 is treated as empty. NULL key with len > 0 yields 0.
 */
uint32_t
MurmurHash3_x86_32(const void *key, int len, uint32_t seed)
{
	const unsigned char *p;
	int nblocks;
	uint32_t h1;
	const uint32_t c1 = 0xcc9e2d51u;
	const uint32_t c2 = 0x1b873593u;
	int i;
	const unsigned char *tail;
	uint32_t k1;
	uint32_t cb;

	if (len <= 0) {
		/* Empty / invalid length: finalize seed with length 0. */
		return b105_fmix32(seed ^ 0u);
	}
	if (key == NULL) {
		return 0u;
	}

	p = (const unsigned char *)key;
	cb = (uint32_t)len;
	nblocks = len / 4;
	h1 = seed;
	k1 = 0u;

	for (i = 0; i < nblocks; i++) {
		uint32_t k = b105_load_le32(p + (size_t)i * 4u);

		k *= c1;
		k = b105_rotl32(k, 15);
		k *= c2;

		h1 ^= k;
		h1 = b105_rotl32(h1, 13);
		h1 = h1 * 5u + 0xe6546b64u;
	}

	tail = p + (size_t)nblocks * 4u;
	switch (len & 3) {
	case 3:
		k1 ^= (uint32_t)tail[2] << 16;
		/* fallthrough */
	case 2:
		k1 ^= (uint32_t)tail[1] << 8;
		/* fallthrough */
	case 1:
		k1 ^= (uint32_t)tail[0];
		k1 *= c1;
		k1 = b105_rotl32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
		break;
	default:
		break;
	}

	h1 ^= cb;
	return b105_fmix32(h1);
}

uint32_t __MurmurHash3_x86_32(const void *key, int len, uint32_t seed)
	__attribute__((alias("MurmurHash3_x86_32")));

/* ---- MurmurHash3 x64_128 ----------------------------------------------- */

/*
 * Writes 16 little-endian bytes (h1 || h2) to out.
 * len < 0 or NULL out: no write. NULL key with len > 0: zeros out.
 */
void
MurmurHash3_x64_128(const void *key, const int len, uint32_t seed, void *out)
{
	const unsigned char *data;
	int nblocks;
	int nLen;
	uint64_t h1;
	uint64_t h2;
	const uint64_t c1 = 0x87c37b91114253d5ull;
	const uint64_t c2 = 0x4cf5ad432745937full;
	int i;
	const unsigned char *tail;
	uint64_t k1;
	uint64_t k2;
	uint64_t clen;
	unsigned char *pOut;

	if (out == NULL) {
		return;
	}
	pOut = (unsigned char *)out;

	/* Negative length → empty. NULL key with positive len → zero out. */
	if (len < 0) {
		nLen = 0;
		data = NULL;
	} else if (len > 0 && key == NULL) {
		b105_store_le64(pOut, 0ull);
		b105_store_le64(pOut + 8, 0ull);
		return;
	} else {
		nLen = len;
		data = (const unsigned char *)key;
	}

	nblocks = nLen / 16;
	h1 = (uint64_t)seed;
	h2 = (uint64_t)seed;
	clen = (uint64_t)(uint32_t)nLen;

	for (i = 0; i < nblocks; i++) {
		k1 = b105_load_le64(data + (size_t)i * 16u);
		k2 = b105_load_le64(data + (size_t)i * 16u + 8u);

		k1 *= c1;
		k1 = b105_rotl64(k1, 31);
		k1 *= c2;
		h1 ^= k1;

		h1 = b105_rotl64(h1, 27);
		h1 += h2;
		h1 = h1 * 5ull + 0x52dce729ull;

		k2 *= c2;
		k2 = b105_rotl64(k2, 33);
		k2 *= c1;
		h2 ^= k2;

		h2 = b105_rotl64(h2, 31);
		h2 += h1;
		h2 = h2 * 5ull + 0x38495ab5ull;
	}

	tail = data + (size_t)nblocks * 16u;
	k1 = 0ull;
	k2 = 0ull;

	switch (nLen & 15) {
	case 15:
		k2 ^= (uint64_t)tail[14] << 48;
		/* fallthrough */
	case 14:
		k2 ^= (uint64_t)tail[13] << 40;
		/* fallthrough */
	case 13:
		k2 ^= (uint64_t)tail[12] << 32;
		/* fallthrough */
	case 12:
		k2 ^= (uint64_t)tail[11] << 24;
		/* fallthrough */
	case 11:
		k2 ^= (uint64_t)tail[10] << 16;
		/* fallthrough */
	case 10:
		k2 ^= (uint64_t)tail[9] << 8;
		/* fallthrough */
	case 9:
		k2 ^= (uint64_t)tail[8];
		k2 *= c2;
		k2 = b105_rotl64(k2, 33);
		k2 *= c1;
		h2 ^= k2;
		/* fallthrough */
	case 8:
		k1 ^= (uint64_t)tail[7] << 56;
		/* fallthrough */
	case 7:
		k1 ^= (uint64_t)tail[6] << 48;
		/* fallthrough */
	case 6:
		k1 ^= (uint64_t)tail[5] << 40;
		/* fallthrough */
	case 5:
		k1 ^= (uint64_t)tail[4] << 32;
		/* fallthrough */
	case 4:
		k1 ^= (uint64_t)tail[3] << 24;
		/* fallthrough */
	case 3:
		k1 ^= (uint64_t)tail[2] << 16;
		/* fallthrough */
	case 2:
		k1 ^= (uint64_t)tail[1] << 8;
		/* fallthrough */
	case 1:
		k1 ^= (uint64_t)tail[0];
		k1 *= c1;
		k1 = b105_rotl64(k1, 31);
		k1 *= c2;
		h1 ^= k1;
		break;
	default:
		break;
	}

	h1 ^= clen;
	h2 ^= clen;

	h1 += h2;
	h2 += h1;

	h1 = b105_fmix64(h1);
	h2 = b105_fmix64(h2);

	h1 += h2;
	h2 += h1;

	b105_store_le64(pOut, h1);
	b105_store_le64(pOut + 8, h2);
}

void __MurmurHash3_x64_128(const void *key, const int len, uint32_t seed,
			   void *out)
	__attribute__((alias("MurmurHash3_x64_128")));
