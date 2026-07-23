/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1769: SHA-512 digest length constant.
 *
 * Surface (unique symbols):
 *   size_t gj_sha512_digest_len(void);
 *     — SHA-512 message digest size (FIPS 180-4): 64 octets
 *       (512 bits). Companion to gj_sha256_digest_len (batch1768).
 *   size_t __gj_sha512_digest_len  (alias)
 *   __libcgj_batch1769_marker = "libcgj-batch1769"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1769_marker[] = "libcgj-batch1769";

/* SHA-512 digest length in octets. */
#define B1769_SHA512_DIGEST_LEN  64u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1769_digest_len(void)
{
	return (size_t)B1769_SHA512_DIGEST_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sha512_digest_len — SHA-512 digest size in octets (64).
 */
size_t
gj_sha512_digest_len(void)
{
	return b1769_digest_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_sha512_digest_len(void)
    __attribute__((alias("gj_sha512_digest_len")));
