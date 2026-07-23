/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5238: pure-value VirtIO descriptor size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_desc_size_u(void);
 *     - Return the VirtIO vring descriptor size in bytes (16).
 *   uint32_t __gj_virtio_desc_size_u  (alias)
 *   __libcgj_batch5238_marker = "libcgj-batch5238"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_desc_size_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5238_marker[] = "libcgj-batch5238";

/* sizeof(struct vring_desc) classic layout: 16 bytes. */
#define B5238_DESC_SIZE  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5238_desc_size(void)
{
	return B5238_DESC_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_desc_size_u - VirtIO vring descriptor size in bytes.
 *
 * Always returns 16. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_desc_size_u(void)
{
	(void)NULL;
	return b5238_desc_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_desc_size_u(void)
    __attribute__((alias("gj_virtio_desc_size_u")));
