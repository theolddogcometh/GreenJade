/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2468: ChaCha20 key length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chacha20_key_len_u(void);
 *     - Returns the fixed ChaCha20 key length in octets (32).
 *   uint32_t __gj_chacha20_key_len_u  (alias)
 *   __libcgj_batch2468_marker = "libcgj-batch2468"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_chacha20_key_len_u surface only; no multi-def. Size constant only —
 * no cipher implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2468_marker[] = "libcgj-batch2468";

/* ChaCha20 key length in octets (RFC 8439). */
#define B2468_CHACHA20_KEY_LEN  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2468_chacha20_key_len(void)
{
	return B2468_CHACHA20_KEY_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chacha20_key_len_u - report ChaCha20 key length in octets.
 *
 * Always returns 32. Soft compile-time crypto size tag; does not encrypt.
 */
uint32_t
gj_chacha20_key_len_u(void)
{
	(void)NULL;
	return b2468_chacha20_key_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chacha20_key_len_u(void)
    __attribute__((alias("gj_chacha20_key_len_u")));
