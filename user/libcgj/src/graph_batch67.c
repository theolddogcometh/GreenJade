/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch67: CRC-64 fill-ins only.
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   crc64 / crc64_ecma / __crc64 / __crc64_ecma  (graph_batch41.c)
 *   XXH64 / __XXH64                               (graph_batch40.c)
 *   crc32c                                        (graph_batch39.c)
 *
 * This TU adds only symbols that were missing:
 *   crc64_update / __crc64_update
 *     — incremental CRC-64/ECMA-182 (poly 0x42F0E1EBA9EA3693, non-reflected,
 *       init 0). Same algorithm as batch41 crc64_ecma; composes with crc64().
 *   crc64_xz / crc64_xz_update / __crc64_xz / __crc64_xz_update
 *     — CRC-64/XZ (same ECMA poly, reflected; init/xorout all-ones).
 *       One-shot: crc64_xz(p, n) == crc64_xz_update(0, p, n).
 *       Check("123456789") == 0x995dc9bbdf1939fa.
 *
 * Marker: __libcgj_batch67_marker = "libcgj-batch67"
 *
 * Freestanding pure C, integer/pointer only (no SSE). Clean-room public
 * algorithms (ISO CRC / xz CRC-64 catalog parameters).
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch67_marker[] = "libcgj-batch67";

/* ---- CRC-64/ECMA-182 incremental (matches batch41 crc64_ecma) ------------ */

/*
 * Poly 0x42F0E1EBA9EA3693, refin=false, refout=false, xorout=0.
 * Check("123456789") = 0x6c40df5f0b497347.
 * Callers start with uCrc=0 for a fresh digest (same as crc64 one-shot).
 */
uint64_t
crc64_update(uint64_t uCrc, const void *pData, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pData;
	const uint64_t poly = 0x42F0E1EBA9EA3693ull;
	size_t i;

	if (p == NULL && cb != 0u) {
		return uCrc;
	}
	for (i = 0; i < cb; i++) {
		unsigned b;

		uCrc ^= (uint64_t)p[i] << 56;
		for (b = 0; b < 8u; b++) {
			if (uCrc & 0x8000000000000000ull) {
				uCrc = (uCrc << 1) ^ poly;
			} else {
				uCrc <<= 1;
			}
		}
	}
	return uCrc;
}

uint64_t __crc64_update(uint64_t c, const void *p, size_t n)
    __attribute__((alias("crc64_update")));

/* ---- CRC-64/XZ (ECMA poly, reflected; lzma-compatible chaining) --------- */

/*
 * Catalog CRC-64/XZ:
 *   width=64 poly=0x42F0E1EBA9EA3693 init=0xFFFFFFFFFFFFFFFF
 *   refin=true refout=true xorout=0xFFFFFFFFFFFFFFFF
 * Reflected poly used bit-serial LSB-first: 0xC96C5795D7870F42.
 *
 * Chaining convention (same as liblzma lzma_crc64): each call does
 *   crc = ~crc; process; return ~crc;
 * so crc64_xz_update(0, data, n) is the one-shot digest, and successive
 * updates pass the previous return value.
 */
uint64_t
crc64_xz_update(uint64_t uCrc, const void *pData, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pData;
	/* Reflected form of 0x42F0E1EBA9EA3693 */
	const uint64_t poly = 0xC96C5795D7870F42ull;
	size_t i;

	if (p == NULL && cb != 0u) {
		return uCrc;
	}

	uCrc = ~uCrc;
	for (i = 0; i < cb; i++) {
		unsigned b;

		uCrc ^= (uint64_t)p[i];
		for (b = 0; b < 8u; b++) {
			if (uCrc & 1ull) {
				uCrc = (uCrc >> 1) ^ poly;
			} else {
				uCrc >>= 1;
			}
		}
	}
	return ~uCrc;
}

uint64_t
crc64_xz(const void *pData, size_t cb)
{
	return crc64_xz_update(0ull, pData, cb);
}

uint64_t __crc64_xz_update(uint64_t c, const void *p, size_t n)
    __attribute__((alias("crc64_xz_update")));
uint64_t __crc64_xz(const void *p, size_t n) __attribute__((alias("crc64_xz")));
