/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1770: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1770(void);
 *     — Returns the compile-time graph batch number for this TU (1770).
 *       Milestone for the TLS record / crypto size exclusive series
 *       (batches 1761–1769: record header, plain/cipher max hints,
 *       GCM/Poly1305 tags, AES/ChaCha20 blocks, SHA-256/512 digests).
 *   uint32_t __gj_batch_id_1770  (alias)
 *   __libcgj_batch1770_marker = "libcgj-batch1770"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1770_marker[] = "libcgj-batch1770";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1770 — report this TU's graph batch number.
 *
 * Always returns 1770. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1770(void)
{
	return 1770u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1770(void)
    __attribute__((alias("gj_batch_id_1770")));
