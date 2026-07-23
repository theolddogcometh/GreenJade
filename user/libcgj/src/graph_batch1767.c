/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1767: ChaCha20 block length constant.
 *
 * Surface (unique symbols):
 *   size_t gj_chacha20_block_len(void);
 *     — ChaCha20 keystream block size (RFC 8439 §2.3): 64 octets
 *       (16 × 32-bit words). Distinct from gj_chacha20_block_counter
 *       (batch202) which emits a block — this is the size constant.
 *   size_t __gj_chacha20_block_len  (alias)
 *   __libcgj_batch1767_marker = "libcgj-batch1767"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1767_marker[] = "libcgj-batch1767";

/* ChaCha20 block size in octets. */
#define B1767_CHACHA_BLOCK_LEN  64u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1767_block_len(void)
{
	return (size_t)B1767_CHACHA_BLOCK_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chacha20_block_len — ChaCha20 keystream block size in octets (64).
 */
size_t
gj_chacha20_block_len(void)
{
	return b1767_block_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_chacha20_block_len(void)
    __attribute__((alias("gj_chacha20_block_len")));
