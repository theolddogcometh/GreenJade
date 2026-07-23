/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2464: MD5 digest length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_md5_digest_len_u(void);
 *     - Returns the fixed MD5 message digest length in octets (16).
 *   uint32_t __gj_md5_digest_len_u  (alias)
 *   __libcgj_batch2464_marker = "libcgj-batch2464"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_md5_digest_len_u surface only; no multi-def. Size constant only —
 * no hash implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2464_marker[] = "libcgj-batch2464";

/* MD5 digest length in octets (RFC 1321). */
#define B2464_MD5_DIGEST_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2464_md5_digest_len(void)
{
	return B2464_MD5_DIGEST_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_md5_digest_len_u - report MD5 digest length in octets.
 *
 * Always returns 16. Soft compile-time crypto size tag; does not hash.
 */
uint32_t
gj_md5_digest_len_u(void)
{
	(void)NULL;
	return b2464_md5_digest_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_md5_digest_len_u(void)
    __attribute__((alias("gj_md5_digest_len_u")));
