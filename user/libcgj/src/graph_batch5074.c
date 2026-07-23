/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5074: SHA-512 digest length (bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_sha512_digest_u(void);
 *     - Returns the SHA-512 output digest length in bytes (64).
 *       FIPS-180-4 §1 / §6.4.
 *   uint32_t __gj_crypto_sha512_digest_u  (alias)
 *   __libcgj_batch5074_marker = "libcgj-batch5074"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_crypto_sha256_digest_u (batch5073) and SHA-512 block size (128) —
 * unique gj_crypto_sha512_digest_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5074_marker[] = "libcgj-batch5074";

/* SHA-512 digest length in octets (FIPS-180-4). */
#define B5074_SHA512_DIGEST  ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5074_sha512_digest(void)
{
	return B5074_SHA512_DIGEST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_sha512_digest_u - report SHA-512 digest length in bytes.
 *
 * Always returns 64. Coincidentally equal to SHA-256 block size but a
 * distinct symbol/semantic. Self-contained; no parent wires.
 */
uint32_t
gj_crypto_sha512_digest_u(void)
{
	(void)NULL;
	return b5074_sha512_digest();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_sha512_digest_u(void)
    __attribute__((alias("gj_crypto_sha512_digest_u")));
