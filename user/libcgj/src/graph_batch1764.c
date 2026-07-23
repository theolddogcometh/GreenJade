/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1764: AES-GCM AEAD authentication tag length.
 *
 * Surface (unique symbols):
 *   size_t gj_aead_tag_len_gcm(void);
 *     — AES-GCM authentication tag size used by TLS (RFC 5116 /
 *       RFC 5288 / RFC 8446): 16 octets.
 *   size_t __gj_aead_tag_len_gcm  (alias)
 *   __libcgj_batch1764_marker = "libcgj-batch1764"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1764_marker[] = "libcgj-batch1764";

/* AES-GCM tag length in octets. */
#define B1764_GCM_TAG_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1764_tag_len(void)
{
	return (size_t)B1764_GCM_TAG_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aead_tag_len_gcm — AES-GCM AEAD tag length in octets (16).
 */
size_t
gj_aead_tag_len_gcm(void)
{
	return b1764_tag_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_aead_tag_len_gcm(void)
    __attribute__((alias("gj_aead_tag_len_gcm")));
