/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5234: pure-value VirtIO DRIVER status (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_status_driver_u(void);
 *     - Return VIRTIO_CONFIG_S_DRIVER status bit value (2).
 *   uint32_t __gj_virtio_status_driver_u  (alias)
 *   __libcgj_batch5234_marker = "libcgj-batch5234"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_status_driver_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5234_marker[] = "libcgj-batch5234";

/* VIRTIO_CONFIG_S_DRIVER */
#define B5234_STATUS_DRIVER  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5234_status_driver(void)
{
	return B5234_STATUS_DRIVER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_status_driver_u - VirtIO DRIVER status bit value.
 *
 * Always returns 2. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_status_driver_u(void)
{
	(void)NULL;
	return b5234_status_driver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_status_driver_u(void)
    __attribute__((alias("gj_virtio_status_driver_u")));
