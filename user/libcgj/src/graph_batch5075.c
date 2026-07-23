/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5075: ChaCha20 / ChaCha20-Poly1305 nonce size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_chacha_nonce_u(void);
 *     - Returns the RFC 8439 ChaCha20 nonce length in bytes (12).
 *   uint32_t __gj_crypto_chacha_nonce_u  (alias)
 *   __libcgj_batch5075_marker = "libcgj-batch5075"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * chacha20_xor / chacha20_block (batch42) nonce array dims —
 * unique gj_crypto_chacha_nonce_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5075_marker[] = "libcgj-batch5075";

/* RFC 8439 ChaCha20 96-bit nonce length in octets. */
#define B5075_CHACHA_NONCE  ((uint32_t)12u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5075_chacha_nonce(void)
{
	return B5075_CHACHA_NONCE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_chacha_nonce_u - report ChaCha20 nonce size in bytes.
 *
 * Always returns 12 (96-bit nonce per RFC 8439). Self-contained;
 * no parent wires.
 */
uint32_t
gj_crypto_chacha_nonce_u(void)
{
	(void)NULL;
	return b5075_chacha_nonce();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_chacha_nonce_u(void)
    __attribute__((alias("gj_crypto_chacha_nonce_u")));
