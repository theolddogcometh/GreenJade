/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9912: VirtIO queue soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_queue_ok_u_9912(void);
 *     - Return 0 (soft lamp: VirtIO virtqueue not claimed ready for
 *       this freestanding continuum). Catalog only; not vring probe.
 *   uint32_t __gj_virtio_queue_ok_u_9912  (alias)
 *   __libcgj_batch9912_marker = "libcgj-batch9912"
 *
 * Exclusive continuum CREATE-ONLY (9911-9920: virtio soft id stubs —
 * virtio_dev_ok_u, virtio_queue_ok_u, virtio_net_ok_u, virtio_blk_ok_u,
 * virtio_gpu_ok_u, virtio_input_ok_u, virtio_console_ok_u, virtio_fs_ok_u,
 * virtio_ready_u, batch_id_9920). Unique gj_virtio_queue_ok_u_9912 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9912_marker[] = "libcgj-batch9912";

/* Soft VirtIO queue lamp (not claimed / soft-off). */
#define B9912_VIRTIO_QUEUE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9912_queue_ok(void)
{
	return B9912_VIRTIO_QUEUE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_queue_ok_u_9912 - soft VirtIO virtqueue readiness lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not touch avail/
 * used rings or queue-select MMIO. No parent wires.
 */
uint32_t
gj_virtio_queue_ok_u_9912(void)
{
	(void)NULL;
	return b9912_queue_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_queue_ok_u_9912(void)
    __attribute__((alias("gj_virtio_queue_ok_u_9912")));
