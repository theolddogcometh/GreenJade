/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5231: pure-value VirtIO magic constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_magic_u(void);
 *     - Return the VirtIO MMIO / legacy magic value 0x74726976
 *       ('virt' little-endian).
 *   uint32_t __gj_virtio_magic_u  (alias)
 *   __libcgj_batch5231_marker = "libcgj-batch5231"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_magic_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5231_marker[] = "libcgj-batch5231";

/* VirtIO magic: little-endian 'virt'. */
#define B5231_MAGIC  ((uint32_t)0x74726976u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5231_magic(void)
{
	return B5231_MAGIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_magic_u - VirtIO device magic constant 0x74726976.
 *
 * Always returns 0x74726976 ('virt' LE). Self-contained; no parent wires.
 */
uint32_t
gj_virtio_magic_u(void)
{
	(void)NULL;
	return b5231_magic();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_magic_u(void)
    __attribute__((alias("gj_virtio_magic_u")));
