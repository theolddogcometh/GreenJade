/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9920: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9920(void);
 *     - Returns the compile-time graph batch number for this TU (9920).
 *   uint32_t __gj_batch_id_9920  (alias)
 *   __libcgj_batch9920_marker = "libcgj-batch9920"
 *
 * Exclusive continuum CREATE-ONLY (9911-9920: virtio soft id stubs —
 * virtio_dev_ok_u, virtio_queue_ok_u, virtio_net_ok_u, virtio_blk_ok_u,
 * virtio_gpu_ok_u, virtio_input_ok_u, virtio_console_ok_u, virtio_fs_ok_u,
 * virtio_ready_u, batch_id_9920). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. Soft
 * pure-data catalog only; no VirtIO I/O. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9920_marker[] = "libcgj-batch9920";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9920_id(void)
{
	return 9920u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9920 - report this TU's graph batch number.
 *
 * Always returns 9920. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9920(void)
{
	(void)NULL;
	return b9920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9920(void)
    __attribute__((alias("gj_batch_id_9920")));
