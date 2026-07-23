/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch265: gj_siphash24 — classic SipHash-2-4.
 *
 * Surface (unique symbols — do NOT collide with sibling siphash exports):
 *   uint64_t gj_siphash24(const void *data, size_t n, const uint8_t key[16]);
 *     — SipHash-2-4 (Aumasson / Bernstein), data-first, 128-bit key,
 *       64-bit tag. NULL key → 16 zero bytes. NULL data with n != 0 → 0.
 *   __gj_siphash24  (alias)
 *   __libcgj_batch265_marker = "libcgj-batch265"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   siphash24 / siphash24_buf / __siphash24           → graph_batch36.c
 *   siphash24_with_key / siphash13_with_key / halfsiphash24 → graph_batch75.c
 *   streaming SipHash context API                     → graph_batch201.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch265_marker[] = "libcgj-batch265";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b265_rol64(uint64_t x, unsigned n)
{
	return (x << n) | (x >> (64u - n));
}

/* Little-endian load of up to 8 bytes (partial OK). */
static uint64_t
b265_load_le64(const unsigned char *p, size_t cb)
{
	uint64_t u = 0;
	size_t i;

	for (i = 0; i < cb && i < 8u; i++) {
		u |= (uint64_t)p[i] << (8u * i);
	}
	return u;
}

/*
 * b265_siphash24 — SipHash-2-4 core.
 *
 * 2 compression rounds per message block, 4 finalization rounds.
 * Key is 128-bit (k0 || k1 little-endian). Output is 64-bit tag.
 */
static uint64_t
b265_siphash24(const unsigned char *pIn, size_t cb, const unsigned char aKey[16])
{
	uint64_t k0 = b265_load_le64(aKey, 8u);
	uint64_t k1 = b265_load_le64(aKey + 8, 8u);
	uint64_t v0 = 0x736f6d6570736575ull ^ k0;
	uint64_t v1 = 0x646f72616e646f6dull ^ k1;
	uint64_t v2 = 0x6c7967656e657261ull ^ k0;
	uint64_t v3 = 0x7465646279746573ull ^ k1;
	uint64_t b;
	size_t nLeft = cb;
	const unsigned char *p = pIn;

#define B265_SIPROUND                                                         \
	do {                                                                  \
		v0 += v1;                                                     \
		v1 = b265_rol64(v1, 13);                                      \
		v1 ^= v0;                                                     \
		v0 = b265_rol64(v0, 32);                                      \
		v2 += v3;                                                     \
		v3 = b265_rol64(v3, 16);                                      \
		v3 ^= v2;                                                     \
		v0 += v3;                                                     \
		v3 = b265_rol64(v3, 21);                                      \
		v3 ^= v0;                                                     \
		v2 += v1;                                                     \
		v1 = b265_rol64(v1, 17);                                      \
		v1 ^= v2;                                                     \
		v2 = b265_rol64(v2, 32);                                      \
	} while (0)

	while (nLeft >= 8u) {
		uint64_t m = b265_load_le64(p, 8u);

		v3 ^= m;
		B265_SIPROUND;
		B265_SIPROUND;
		v0 ^= m;
		p += 8u;
		nLeft -= 8u;
	}

	/* Final partial block: length (mod 256) in high byte + remaining bytes. */
	b = ((uint64_t)cb) << 56;
	b |= b265_load_le64(p, nLeft);
	v3 ^= b;
	B265_SIPROUND;
	B265_SIPROUND;
	v0 ^= b;

	v2 ^= 0xffull;
	B265_SIPROUND;
	B265_SIPROUND;
	B265_SIPROUND;
	B265_SIPROUND;
#undef B265_SIPROUND

	return v0 ^ v1 ^ v2 ^ v3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_siphash24 — classic SipHash-2-4, data-first API.
 *
 * Edge cases:
 *   key == NULL              → treat as 16 zero key bytes
 *   data == NULL && n != 0   → 0
 *   data == NULL && n == 0   → hash of empty message under key
 */
uint64_t
gj_siphash24(const void *data, size_t n, const uint8_t key[16])
{
	static const unsigned char aZero[16];
	const unsigned char *pKey;

	if (key == NULL) {
		pKey = aZero;
	} else {
		pKey = (const unsigned char *)key;
	}
	if (data == NULL && n != 0u) {
		return 0;
	}
	return b265_siphash24((const unsigned char *)data, n, pKey);
}

uint64_t __gj_siphash24(const void *data, size_t n, const uint8_t key[16])
    __attribute__((alias("gj_siphash24")));
