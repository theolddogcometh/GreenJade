/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2461: SHA-1 digest length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sha1_digest_len_u(void);
 *     - Returns the fixed SHA-1 message digest length in octets (20).
 *   uint32_t __gj_sha1_digest_len_u  (alias)
 *   __libcgj_batch2461_marker = "libcgj-batch2461"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_sha1_digest_len_u surface only; no multi-def. Size constant only —
 * no hash implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2461_marker[] = "libcgj-batch2461";

/* SHA-1 digest length in octets (FIPS 180-4). */
#define B2461_SHA1_DIGEST_LEN  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2461_sha1_digest_len(void)
{
	return B2461_SHA1_DIGEST_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sha1_digest_len_u - report SHA-1 digest length in octets.
 *
 * Always returns 20. Soft compile-time crypto size tag; does not hash.
 */
uint32_t
gj_sha1_digest_len_u(void)
{
	(void)NULL;
	return b2461_sha1_digest_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sha1_digest_len_u(void)
    __attribute__((alias("gj_sha1_digest_len_u")));
