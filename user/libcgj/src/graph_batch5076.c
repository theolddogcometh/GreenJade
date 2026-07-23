/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5076: Poly1305 authentication tag size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_poly1305_tag_u(void);
 *     - Returns the Poly1305 MAC tag length in bytes (16).
 *       RFC 8439 §2.5 / §2.8.
 *   uint32_t __gj_crypto_poly1305_tag_u  (alias)
 *   __libcgj_batch5076_marker = "libcgj-batch5076"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * poly1305_auth (batch42) and chacha20_poly1305_* (batch70) —
 * unique gj_crypto_poly1305_tag_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5076_marker[] = "libcgj-batch5076";

/* Poly1305 tag length in octets (RFC 8439). */
#define B5076_POLY1305_TAG  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5076_poly1305_tag(void)
{
	return B5076_POLY1305_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_poly1305_tag_u - report Poly1305 tag size in bytes.
 *
 * Always returns 16. Coincidentally equal to AES block size but a
 * distinct symbol/semantic. Self-contained; no parent wires.
 */
uint32_t
gj_crypto_poly1305_tag_u(void)
{
	(void)NULL;
	return b5076_poly1305_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_poly1305_tag_u(void)
    __attribute__((alias("gj_crypto_poly1305_tag_u")));
