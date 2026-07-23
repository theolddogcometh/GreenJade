/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2470: milestone 2470 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2470(void);
 *     - Returns the compile-time graph batch number for this TU (2470).
 *   uint32_t __gj_batch_id_2470  (alias)
 *   __libcgj_batch2470_marker = "libcgj-batch2470"
 *
 * Milestone for the crypto constants exclusive wave (batches 2461-2469:
 * sha1_digest_len, sha256_block_len, sha512_block_len, md5_digest_len,
 * blake2b_digest_max, aes128_key_len, aes256_key_len, chacha20_key_len,
 * poly1305_tag_len). Does NOT redefine gj_batch_id / prior batch_id_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2470_marker[] = "libcgj-batch2470";

/* Milestone batch identity for the crypto constants wave. */
#define B2470_BATCH_ID  2470u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2470_id(void)
{
	return B2470_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2470 - report this TU's graph batch number.
 *
 * Always returns 2470. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2470(void)
{
	(void)NULL;
	return b2470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2470(void)
    __attribute__((alias("gj_batch_id_2470")));
