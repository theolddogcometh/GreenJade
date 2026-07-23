/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4185: 32-bit XOR reduce over a byte buffer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xor_reduce32_u(const uint8_t *p, size_t n);
 *     - XOR little-endian 4-byte groups from p, then XOR the tail
 *       (0..3 remaining bytes) as a little-endian partial word.
 *       NULL or n == 0 → 0.
 *   uint32_t __gj_xor_reduce32_u  (alias)
 *   __libcgj_batch4185_marker = "libcgj-batch4185"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_xor_reduce32_u surface only; no multi-def.
 * Distinct from gj_xor_reduce8_u (batch4184) and gj_u32_xor_reduce
 * (u32 array, batch1247). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4185_marker[] = "libcgj-batch4185";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load a little-endian uint32 from four octets (no alignment needed). */
static uint32_t
b4185_load_le32(const uint8_t *p)
{
	return (uint32_t)p[0]
	    | ((uint32_t)p[1] << 8)
	    | ((uint32_t)p[2] << 16)
	    | ((uint32_t)p[3] << 24);
}

/*
 * XOR full LE 32-bit groups, then XOR a LE partial word for the tail.
 */
static uint32_t
b4185_xor32(const uint8_t *pData, size_t cbN)
{
	uint32_t u32Acc;
	uint32_t u32Tail;
	size_t iOff;
	size_t i;

	u32Acc = 0u;
	iOff = 0u;
	while (iOff + 4u <= cbN) {
		u32Acc ^= b4185_load_le32(pData + iOff);
		iOff += 4u;
	}

	/* Tail: remaining 0..3 bytes as little-endian partial word. */
	u32Tail = 0u;
	for (i = 0u; iOff + i < cbN; i++) {
		u32Tail |= (uint32_t)pData[iOff + i] << (i * 8u);
	}
	u32Acc ^= u32Tail;
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor_reduce32_u - XOR-fold n bytes via 4-byte LE groups + tail.
 *
 * p: byte buffer (NULL only valid when n == 0)
 * n: length in bytes
 *
 * Returns the 32-bit XOR reduction. NULL or empty yields 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_xor_reduce32_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL || n == 0u) {
		return 0u;
	}
	return b4185_xor32(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xor_reduce32_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_xor_reduce32_u")));
