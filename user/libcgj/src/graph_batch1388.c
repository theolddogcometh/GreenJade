/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1388: pack up to 8 bytes little-endian into a u64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pack_bytes_le(const uint8_t *bytes, size_t n);
 *     — Interpret min(n, 8) bytes as a little-endian integer: bytes[0]
 *       is the least significant octet. Result is right-aligned in the
 *       low bits of the return value. NULL bytes or n == 0 → 0.
 *   uint64_t __gj_u64_pack_bytes_le  (alias)
 *   __libcgj_batch1388_marker = "libcgj-batch1388"
 *
 * Distinct from gj_unpack_u64le / pack_bytes_be. Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1388_marker[] = "libcgj-batch1388";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Right-aligned little-endian pack of cN bytes (cN already capped to 0..8).
 * bytes[i] lands in bits [8*i .. 8*i+7].
 */
static uint64_t
b1388_pack_le(const uint8_t *pBytes, size_t cN)
{
	uint64_t u64Out;
	size_t i;

	u64Out = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Out |= ((uint64_t)pBytes[i]) << (i * 8u);
	}
	return u64Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pack_bytes_le — pack up to 8 octets as a little-endian integer.
 *
 * pBytes: source buffer (or NULL → 0)
 * cN:     number of bytes available; only min(cN, 8) are consumed
 */
uint64_t
gj_u64_pack_bytes_le(const uint8_t *pBytes, size_t cN)
{
	size_t cUse;

	if (pBytes == NULL || cN == 0u) {
		return 0ull;
	}
	cUse = (cN > 8u) ? 8u : cN;
	return b1388_pack_le(pBytes, cUse);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pack_bytes_le(const uint8_t *pBytes, size_t cN)
    __attribute__((alias("gj_u64_pack_bytes_le")));
