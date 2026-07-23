/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9917: VirtIO-console soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_console_ok_u_9917(void);
 *     - Return 0 (soft lamp: VirtIO console device not claimed present
 *       for this freestanding continuum). Catalog only; not console probe.
 *   uint32_t __gj_virtio_console_ok_u_9917  (alias)
 *   __libcgj_batch9917_marker = "libcgj-batch9917"
 *
 * Exclusive continuum CREATE-ONLY (9911-9920: virtio soft id stubs —
 * virtio_dev_ok_u, virtio_queue_ok_u, virtio_net_ok_u, virtio_blk_ok_u,
 * virtio_gpu_ok_u, virtio_input_ok_u, virtio_console_ok_u, virtio_fs_ok_u,
 * virtio_ready_u, batch_id_9920). Unique gj_virtio_console_ok_u_9917
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9917_marker[] = "libcgj-batch9917";

/* Soft VirtIO-console lamp (not claimed / soft-off). */
#define B9917_VIRTIO_CONSOLE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9917_console_ok(void)
{
	return B9917_VIRTIO_CONSOLE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_console_ok_u_9917 - soft VirtIO-console device presence lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not open ports or
 * touch receiveq/transmitq. No parent wires.
 */
uint32_t
gj_virtio_console_ok_u_9917(void)
{
	(void)NULL;
	return b9917_console_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_console_ok_u_9917(void)
    __attribute__((alias("gj_virtio_console_ok_u_9917")));
