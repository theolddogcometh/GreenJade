/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1768: SHA-256 digest length constant.
 *
 * Surface (unique symbols):
 *   size_t gj_sha256_digest_len(void);
 *     — SHA-256 message digest size (FIPS 180-4): 32 octets
 *       (256 bits). Used by TLS 1.2/1.3 handshake and HKDF labels.
 *   size_t __gj_sha256_digest_len  (alias)
 *   __libcgj_batch1768_marker = "libcgj-batch1768"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1768_marker[] = "libcgj-batch1768";

/* SHA-256 digest length in octets. */
#define B1768_SHA256_DIGEST_LEN  32u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1768_digest_len(void)
{
	return (size_t)B1768_SHA256_DIGEST_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sha256_digest_len — SHA-256 digest size in octets (32).
 */
size_t
gj_sha256_digest_len(void)
{
	return b1768_digest_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_sha256_digest_len(void)
    __attribute__((alias("gj_sha256_digest_len")));
