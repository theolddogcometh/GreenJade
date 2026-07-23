/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5071: AES block size constant (bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_aes_block_u(void);
 *     - Returns the AES block size in bytes (16). FIPS-197 / Rijndael
 *       fixed block width for AES-128/192/256.
 *   uint32_t __gj_crypto_aes_block_u  (alias)
 *   __libcgj_batch5071_marker = "libcgj-batch5071"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080: crypto_aes_block_u,
 * crypto_sha256_block_u, crypto_sha256_digest_u, crypto_sha512_digest_u,
 * crypto_chacha_nonce_u, crypto_poly1305_tag_u, crypto_is_pow2_block_u,
 * crypto_pad_pkcs7_len_u, crypto_unpad_ok_u, batch_id_5080).
 * Distinct from AES_BLOCK_SIZE (batch74) and internal cipher sizes —
 * unique gj_crypto_aes_block_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5071_marker[] = "libcgj-batch5071";

/* AES block size in octets (FIPS-197). */
#define B5071_AES_BLOCK  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5071_aes_block(void)
{
	return B5071_AES_BLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_aes_block_u - report AES block size in bytes.
 *
 * Always returns 16. Useful for buffer sizing and padding probes without
 * hard-coding the constant at call sites.
 * Self-contained; no parent wires.
 */
uint32_t
gj_crypto_aes_block_u(void)
{
	(void)NULL;
	return b5071_aes_block();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_aes_block_u(void)
    __attribute__((alias("gj_crypto_aes_block_u")));
