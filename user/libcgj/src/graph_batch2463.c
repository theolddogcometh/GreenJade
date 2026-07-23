/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2463: SHA-512 block length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sha512_block_len_u(void);
 *     - Returns the fixed SHA-512 compression block length in octets (128).
 *   uint32_t __gj_sha512_block_len_u  (alias)
 *   __libcgj_batch2463_marker = "libcgj-batch2463"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_sha512_block_len_u surface only; no multi-def. Size constant only —
 * no hash implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2463_marker[] = "libcgj-batch2463";

/* SHA-512 message block length in octets (FIPS 180-4). */
#define B2463_SHA512_BLOCK_LEN  128u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2463_sha512_block_len(void)
{
	return B2463_SHA512_BLOCK_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sha512_block_len_u - report SHA-512 block length in octets.
 *
 * Always returns 128. Soft compile-time crypto size tag; does not hash.
 */
uint32_t
gj_sha512_block_len_u(void)
{
	(void)NULL;
	return b2463_sha512_block_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sha512_block_len_u(void)
    __attribute__((alias("gj_sha512_block_len_u")));
