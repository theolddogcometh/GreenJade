/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5078: PKCS #7 pad-length calculator.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_pad_pkcs7_len_u(uint32_t len, uint32_t block);
 *     - Returns the number of PKCS #7 pad bytes required to round len
 *       up to the next multiple of block. Always in [1, block] when
 *       block is in [1, 255]; returns 0 when block is 0 or > 255.
 *   uint32_t __gj_crypto_pad_pkcs7_len_u  (alias)
 *   __libcgj_batch5078_marker = "libcgj-batch5078"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_pkcs7_pad (batch134) which mutates a buffer — this is length-only.
 * Unique gj_crypto_pad_pkcs7_len_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5078_marker[] = "libcgj-batch5078";

/* PKCS #7 pad octet is a single byte → block in [1, 255]. */
#define B5078_PKCS7_BLOCK_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5078_pad_pkcs7_len(uint32_t u32Len, uint32_t u32Block)
{
	uint32_t u32Rem;

	if (u32Block == 0u || u32Block > B5078_PKCS7_BLOCK_MAX) {
		return 0u;
	}
	u32Rem = u32Len % u32Block;
	if (u32Rem == 0u) {
		return u32Block;
	}
	return u32Block - u32Rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_pad_pkcs7_len_u - PKCS #7 pad byte count for (len, block).
 *
 * len:   current message length in bytes
 * block: cipher block size in bytes (must be 1..255)
 *
 * Returns block - (len % block), or block when len is already a multiple
 * of block. Returns 0 if block is invalid (0 or > 255). Does not write
 * memory. Self-contained; no parent wires.
 */
uint32_t
gj_crypto_pad_pkcs7_len_u(uint32_t u32Len, uint32_t u32Block)
{
	(void)NULL;
	return b5078_pad_pkcs7_len(u32Len, u32Block);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_pad_pkcs7_len_u(uint32_t u32Len, uint32_t u32Block)
    __attribute__((alias("gj_crypto_pad_pkcs7_len_u")));
