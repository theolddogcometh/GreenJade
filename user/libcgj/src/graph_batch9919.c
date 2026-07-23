/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9919: VirtIO continuum ready lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_ready_u_9919(void);
 *     - Return 1 (soft lamp: 9911-9920 virtio soft id stubs continuum
 *       ready). Catalog presence only; not a live VirtIO device probe.
 *   uint32_t __gj_virtio_ready_u_9919  (alias)
 *   __libcgj_batch9919_marker = "libcgj-batch9919"
 *
 * Exclusive continuum CREATE-ONLY (9911-9920: virtio soft id stubs —
 * virtio_dev_ok_u, virtio_queue_ok_u, virtio_net_ok_u, virtio_blk_ok_u,
 * virtio_gpu_ok_u, virtio_input_ok_u, virtio_console_ok_u, virtio_fs_ok_u,
 * virtio_ready_u, batch_id_9920). Unique gj_virtio_ready_u_9919 surface
 * only; no multi-def. Does not call sibling virtio_* symbols. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9919_marker[] = "libcgj-batch9919";

/* Soft continuum-ready lamp for virtio soft id stubs wave. */
#define B9919_VIRTIO_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9919_ready(void)
{
	return B9919_VIRTIO_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_ready_u_9919 - soft VirtIO continuum ready lamp.
 *
 * Always returns 1. Pure-data product tag that the 9911-9920 virtio soft
 * id stubs exclusive wave is present. Does not call sibling symbols or
 * probe device MMIO/PCI. No parent wires.
 */
uint32_t
gj_virtio_ready_u_9919(void)
{
	(void)NULL;
	return b9919_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_ready_u_9919(void)
    __attribute__((alias("gj_virtio_ready_u_9919")));
