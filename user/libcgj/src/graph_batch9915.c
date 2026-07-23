/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9915: VirtIO-gpu soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_gpu_ok_u_9915(void);
 *     - Return 0 (soft lamp: VirtIO GPU device not claimed present for
 *       this freestanding continuum). Catalog only; not GPU probe.
 *   uint32_t __gj_virtio_gpu_ok_u_9915  (alias)
 *   __libcgj_batch9915_marker = "libcgj-batch9915"
 *
 * Exclusive continuum CREATE-ONLY (9911-9920: virtio soft id stubs —
 * virtio_dev_ok_u, virtio_queue_ok_u, virtio_net_ok_u, virtio_blk_ok_u,
 * virtio_gpu_ok_u, virtio_input_ok_u, virtio_console_ok_u, virtio_fs_ok_u,
 * virtio_ready_u, batch_id_9920). Unique gj_virtio_gpu_ok_u_9915 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9915_marker[] = "libcgj-batch9915";

/* Soft VirtIO-gpu lamp (not claimed / soft-off). */
#define B9915_VIRTIO_GPU_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9915_gpu_ok(void)
{
	return B9915_VIRTIO_GPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_gpu_ok_u_9915 - soft VirtIO-gpu device presence lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not issue
 * VIRTIO_GPU_CMD_* or touch scanout resources. No parent wires.
 */
uint32_t
gj_virtio_gpu_ok_u_9915(void)
{
	(void)NULL;
	return b9915_gpu_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_gpu_ok_u_9915(void)
    __attribute__((alias("gj_virtio_gpu_ok_u_9915")));
