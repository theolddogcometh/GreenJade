/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6558: VirtIO feature-mask AND / has-all.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_mask_and_6558(uint32_t a, uint32_t b);
 *     - Return a & b (soft intersection of two VirtIO feature words).
 *   uint32_t gj_virtio_feat_has_all_6558(uint32_t mask, uint32_t need);
 *     - Return 1 if (mask & need) == need (all required bits present).
 *   uint32_t __gj_virtio_feat_mask_and_6558  (alias)
 *   uint32_t __gj_virtio_feat_has_all_6558  (alias)
 *   __libcgj_batch6558_marker = "libcgj-batch6558"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs).
 * Unique gj_virtio_feat_*_6558 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6558_marker[] = "libcgj-batch6558";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6558_and(uint32_t u32A, uint32_t u32B)
{
	return u32A & u32B;
}

static uint32_t
b6558_has_all(uint32_t u32Mask, uint32_t u32Need)
{
	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_mask_and_6558 - bitwise AND of two VirtIO feature masks.
 *
 * a, b: soft VirtIO feature bitmasks
 *
 * Returns a & b. Soft pure-data intersect (e.g. negotiated = device &
 * driver); no VirtIO device.
 */
uint32_t
gj_virtio_feat_mask_and_6558(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6558_and(u32A, u32B);
}

/*
 * gj_virtio_feat_has_all_6558 - true if mask covers all bits in need.
 *
 * mask: soft VirtIO feature bitmask (available / negotiated)
 * need: required feature bits
 *
 * Returns 1 when every bit of need is set in mask; else 0.
 */
uint32_t
gj_virtio_feat_has_all_6558(uint32_t u32Mask, uint32_t u32Need)
{
	return b6558_has_all(u32Mask, u32Need);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_virtio_feat_mask_and_6558(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_virtio_feat_mask_and_6558")));

uint32_t __gj_virtio_feat_has_all_6558(uint32_t u32Mask, uint32_t u32Need)
    __attribute__((alias("gj_virtio_feat_has_all_6558")));
