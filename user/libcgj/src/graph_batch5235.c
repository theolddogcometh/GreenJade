/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5235: pure-value VirtIO DRIVER_OK status (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_status_ok_u(void);
 *     - Return VIRTIO_CONFIG_S_DRIVER_OK status bit value (4).
 *   uint32_t __gj_virtio_status_ok_u  (alias)
 *   __libcgj_batch5235_marker = "libcgj-batch5235"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_status_ok_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5235_marker[] = "libcgj-batch5235";

/* VIRTIO_CONFIG_S_DRIVER_OK */
#define B5235_STATUS_OK  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5235_status_ok(void)
{
	return B5235_STATUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_status_ok_u - VirtIO DRIVER_OK status bit value.
 *
 * Always returns 4. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_status_ok_u(void)
{
	(void)NULL;
	return b5235_status_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_status_ok_u(void)
    __attribute__((alias("gj_virtio_status_ok_u")));
