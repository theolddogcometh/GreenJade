/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5080: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5080(void);
 *     - Returns the compile-time graph batch number for this TU (5080).
 *   uint32_t __gj_batch_id_5080  (alias)
 *   __libcgj_batch5080_marker = "libcgj-batch5080"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080: crypto_aes_block_u,
 * crypto_sha256_block_u, crypto_sha256_digest_u, crypto_sha512_digest_u,
 * crypto_chacha_nonce_u, crypto_poly1305_tag_u, crypto_is_pow2_block_u,
 * crypto_pad_pkcs7_len_u, crypto_unpad_ok_u, batch_id_5080).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5080_marker[] = "libcgj-batch5080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5080_id(void)
{
	return 5080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5080 - report this TU's graph batch number.
 *
 * Always returns 5080. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5080(void)
{
	(void)NULL;
	return b5080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5080(void)
    __attribute__((alias("gj_batch_id_5080")));
