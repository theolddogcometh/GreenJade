/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5072: SHA-256 compression block size (bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_sha256_block_u(void);
 *     - Returns the SHA-256 message-schedule block size in bytes (64).
 *       FIPS-180-4 §5.2.1 / §6.2.
 *   uint32_t __gj_crypto_sha256_block_u  (alias)
 *   __libcgj_batch5072_marker = "libcgj-batch5072"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_crypto_sha256_digest_u (batch5073) and SHA-512 block sizes —
 * unique gj_crypto_sha256_block_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5072_marker[] = "libcgj-batch5072";

/* SHA-256 input block size in octets (FIPS-180-4). */
#define B5072_SHA256_BLOCK  ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5072_sha256_block(void)
{
	return B5072_SHA256_BLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_sha256_block_u - report SHA-256 block size in bytes.
 *
 * Always returns 64. Distinct from the 32-byte digest length
 * (gj_crypto_sha256_digest_u). Self-contained; no parent wires.
 */
uint32_t
gj_crypto_sha256_block_u(void)
{
	(void)NULL;
	return b5072_sha256_block();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_sha256_block_u(void)
    __attribute__((alias("gj_crypto_sha256_block_u")));
