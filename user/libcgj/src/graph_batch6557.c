/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6557: VirtIO feature-mask OR merge.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_mask_or_6557(uint32_t a, uint32_t b);
 *     - Return a | b (soft merge of two VirtIO feature words).
 *   uint32_t __gj_virtio_feat_mask_or_6557  (alias)
 *   __libcgj_batch6557_marker = "libcgj-batch6557"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs).
 * Unique gj_virtio_feat_mask_or_6557 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6557_marker[] = "libcgj-batch6557";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6557_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_mask_or_6557 - bitwise OR of two VirtIO feature masks.
 *
 * a, b: soft VirtIO feature bitmasks
 *
 * Returns a | b. Soft pure-data merge (e.g. negotiated = offered | want
 * style bookkeeping); no VirtIO device.
 */
uint32_t
gj_virtio_feat_mask_or_6557(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6557_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_feat_mask_or_6557(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_virtio_feat_mask_or_6557")));
