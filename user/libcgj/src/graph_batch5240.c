/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5240: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5240(void);
 *     - Returns the compile-time graph batch number for this TU (5240).
 *   uint32_t __gj_batch_id_5240  (alias)
 *   __libcgj_batch5240_marker = "libcgj-batch5240"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5240_marker[] = "libcgj-batch5240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5240_id(void)
{
	return 5240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5240 - report this TU's graph batch number.
 *
 * Always returns 5240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5240(void)
{
	(void)NULL;
	return b5240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5240(void)
    __attribute__((alias("gj_batch_id_5240")));
