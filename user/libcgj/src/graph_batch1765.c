/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1765: Poly1305 AEAD authentication tag length.
 *
 * Surface (unique symbols):
 *   size_t gj_aead_tag_len_poly1305(void);
 *     — Poly1305 authenticator size used by ChaCha20-Poly1305
 *       (RFC 8439 / RFC 7905 / RFC 8446): 16 octets.
 *   size_t __gj_aead_tag_len_poly1305  (alias)
 *   __libcgj_batch1765_marker = "libcgj-batch1765"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Distinct from
 * gj_aead_tag_len_gcm (batch1764) — same numeric width, separate
 * algorithm surface. Does NOT redefine prior batch symbols.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1765_marker[] = "libcgj-batch1765";

/* Poly1305 tag length in octets. */
#define B1765_POLY_TAG_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1765_tag_len(void)
{
	return (size_t)B1765_POLY_TAG_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aead_tag_len_poly1305 — Poly1305 AEAD tag length in octets (16).
 */
size_t
gj_aead_tag_len_poly1305(void)
{
	return b1765_tag_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_aead_tag_len_poly1305(void)
    __attribute__((alias("gj_aead_tag_len_poly1305")));
