/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1763: TLS max ciphertext fragment hint.
 *
 * Surface (unique symbols):
 *   size_t gj_tls_max_cipher_hint(void);
 *     — Conventional maximum TLSCiphertext.fragment length for classic
 *       TLS record expansion: 2^14 + 2048 = 18432 octets
 *       (RFC 5246 §6.2.3). A buffer-sizing hint; TLS 1.3 AEAD limits
 *       are tighter (2^14 + 256) — callers apply suite policy.
 *   size_t __gj_tls_max_cipher_hint  (alias)
 *   __libcgj_batch1763_marker = "libcgj-batch1763"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1763_marker[] = "libcgj-batch1763";

/* TLSCiphertext fragment max (2^14 + 2048). */
#define B1763_TLS_MAX_CIPHER  18432u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1763_max_cipher(void)
{
	return (size_t)B1763_TLS_MAX_CIPHER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_max_cipher_hint — max ciphertext fragment size hint (18432).
 */
size_t
gj_tls_max_cipher_hint(void)
{
	return b1763_max_cipher();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tls_max_cipher_hint(void)
    __attribute__((alias("gj_tls_max_cipher_hint")));
