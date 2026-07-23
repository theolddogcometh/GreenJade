/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5232: pure-value VirtIO version-1 constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_ver1_u(void);
 *     - Return VirtIO version 1 (legacy / modern v1 marker value 1).
 *   uint32_t __gj_virtio_ver1_u  (alias)
 *   __libcgj_batch5232_marker = "libcgj-batch5232"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_ver1_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5232_marker[] = "libcgj-batch5232";

/* VirtIO version 1. */
#define B5232_VER1  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5232_ver1(void)
{
	return B5232_VER1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_ver1_u - VirtIO version-1 constant.
 *
 * Always returns 1. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_ver1_u(void)
{
	(void)NULL;
	return b5232_ver1();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_ver1_u(void)
    __attribute__((alias("gj_virtio_ver1_u")));
