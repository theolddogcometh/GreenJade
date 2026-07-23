/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1387: pack up to 8 bytes big-endian into a u64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pack_bytes_be(const uint8_t *bytes, size_t n);
 *     — Interpret min(n, 8) bytes as a big-endian integer: bytes[0] is
 *       the most significant octet among those consumed. Result is
 *       right-aligned in the low bits of the return value. NULL bytes
 *       or n == 0 → 0.
 *   uint64_t __gj_u64_pack_bytes_be  (alias)
 *   __libcgj_batch1387_marker = "libcgj-batch1387"
 *
 * Distinct from gj_unpack_u64be (batch516, fixed 8 bytes). Unique
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1387_marker[] = "libcgj-batch1387";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Right-aligned big-endian pack of cN bytes (cN already capped to 0..8).
 * Each step shifts left 8 and folds in the next octet (MSB first).
 */
static uint64_t
b1387_pack_be(const uint8_t *pBytes, size_t cN)
{
	uint64_t u64Out;
	size_t i;

	u64Out = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Out = (u64Out << 8) | (uint64_t)pBytes[i];
	}
	return u64Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pack_bytes_be — pack up to 8 octets as a big-endian integer.
 *
 * pBytes: source buffer (or NULL → 0)
 * cN:     number of bytes available; only min(cN, 8) are consumed
 */
uint64_t
gj_u64_pack_bytes_be(const uint8_t *pBytes, size_t cN)
{
	size_t cUse;

	if (pBytes == NULL || cN == 0u) {
		return 0ull;
	}
	cUse = (cN > 8u) ? 8u : cN;
	return b1387_pack_be(pBytes, cUse);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pack_bytes_be(const uint8_t *pBytes, size_t cN)
    __attribute__((alias("gj_u64_pack_bytes_be")));
