/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5073: SHA-256 digest length (bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_sha256_digest_u(void);
 *     - Returns the SHA-256 output digest length in bytes (32).
 *       FIPS-180-4 §1 / §6.2.
 *   uint32_t __gj_crypto_sha256_digest_u  (alias)
 *   __libcgj_batch5073_marker = "libcgj-batch5073"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_crypto_sha256_block_u (batch5072) and gj_crypto_sha512_digest_u
 * (batch5074) — unique gj_crypto_sha256_digest_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5073_marker[] = "libcgj-batch5073";

/* SHA-256 digest length in octets (FIPS-180-4). */
#define B5073_SHA256_DIGEST  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5073_sha256_digest(void)
{
	return B5073_SHA256_DIGEST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_sha256_digest_u - report SHA-256 digest length in bytes.
 *
 * Always returns 32. Distinct from the 64-byte compression block
 * (gj_crypto_sha256_block_u). Self-contained; no parent wires.
 */
uint32_t
gj_crypto_sha256_digest_u(void)
{
	(void)NULL;
	return b5073_sha256_digest();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_sha256_digest_u(void)
    __attribute__((alias("gj_crypto_sha256_digest_u")));
