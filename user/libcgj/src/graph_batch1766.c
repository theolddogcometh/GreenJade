/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1766: AES block length constant.
 *
 * Surface (unique symbols):
 *   size_t gj_aes_block_len(void);
 *     — AES block size (FIPS 197): always 16 octets, independent of
 *       key length (128/192/256-bit keys all use a 128-bit block).
 *   size_t __gj_aes_block_len  (alias)
 *   __libcgj_batch1766_marker = "libcgj-batch1766"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1766_marker[] = "libcgj-batch1766";

/* AES block size in octets (Nb * 4 with Nb = 4). */
#define B1766_AES_BLOCK_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1766_block_len(void)
{
	return (size_t)B1766_AES_BLOCK_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aes_block_len — AES block size in octets (16).
 */
size_t
gj_aes_block_len(void)
{
	return b1766_block_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_aes_block_len(void)
    __attribute__((alias("gj_aes_block_len")));
