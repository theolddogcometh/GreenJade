/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5079: PKCS #7 pad-value range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_unpad_ok_u(uint32_t pad, uint32_t block);
 *     - Returns 1 if pad is a legal PKCS #7 trailing pad length for the
 *       given block size (pad in 1..block inclusive and block in 1..255);
 *       else 0. Does not inspect buffer contents.
 *   uint32_t __gj_crypto_unpad_ok_u  (alias)
 *   __libcgj_batch5079_marker = "libcgj-batch5079"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_pkcs7_unpad (batch134) which validates full pad runs in memory —
 * unique gj_crypto_unpad_ok_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5079_marker[] = "libcgj-batch5079";

/* PKCS #7 pad octet is a single byte → block in [1, 255]. */
#define B5079_PKCS7_BLOCK_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5079_unpad_ok(uint32_t u32Pad, uint32_t u32Block)
{
	if (u32Block == 0u || u32Block > B5079_PKCS7_BLOCK_MAX) {
		return 0u;
	}
	if (u32Pad == 0u || u32Pad > u32Block) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_unpad_ok_u - validate a PKCS #7 pad length against block.
 *
 * pad:   candidate pad length (typically the last ciphertext byte)
 * block: cipher block size in bytes (must be 1..255)
 *
 * Returns 1 when pad is in [1, block] and block is legal; else 0.
 * Scalar range check only — does not verify pad-byte repetition.
 * Self-contained; no parent wires.
 */
uint32_t
gj_crypto_unpad_ok_u(uint32_t u32Pad, uint32_t u32Block)
{
	(void)NULL;
	return b5079_unpad_ok(u32Pad, u32Block);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_unpad_ok_u(uint32_t u32Pad, uint32_t u32Block)
    __attribute__((alias("gj_crypto_unpad_ok_u")));
