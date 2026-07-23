/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2466: AES-128 key length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_aes128_key_len_u(void);
 *     - Returns the fixed AES-128 key length in octets (16).
 *   uint32_t __gj_aes128_key_len_u  (alias)
 *   __libcgj_batch2466_marker = "libcgj-batch2466"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_aes128_key_len_u surface only; no multi-def. Size constant only —
 * no cipher implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2466_marker[] = "libcgj-batch2466";

/* AES-128 key length in octets (FIPS 197). */
#define B2466_AES128_KEY_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2466_aes128_key_len(void)
{
	return B2466_AES128_KEY_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aes128_key_len_u - report AES-128 key length in octets.
 *
 * Always returns 16. Soft compile-time crypto size tag; does not encrypt.
 */
uint32_t
gj_aes128_key_len_u(void)
{
	(void)NULL;
	return b2466_aes128_key_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aes128_key_len_u(void)
    __attribute__((alias("gj_aes128_key_len_u")));
