/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5237: pure-value VirtIO queue align constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_q_align_u(void);
 *     - Return the VirtIO virtqueue page alignment constant (4096).
 *   uint32_t __gj_virtio_q_align_u  (alias)
 *   __libcgj_batch5237_marker = "libcgj-batch5237"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_q_align_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5237_marker[] = "libcgj-batch5237";

/* VirtIO virtqueue page alignment (bytes). */
#define B5237_Q_ALIGN  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5237_q_align(void)
{
	return B5237_Q_ALIGN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_q_align_u - VirtIO queue page-alignment constant.
 *
 * Always returns 4096. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_q_align_u(void)
{
	(void)NULL;
	return b5237_q_align();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_q_align_u(void)
    __attribute__((alias("gj_virtio_q_align_u")));
