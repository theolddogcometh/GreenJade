/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5239: pure-value VirtIO available-flag probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_is_avail_u(uint32_t flags);
 *     - Return 1 if flags has bit0 set (available / avail-ring flag
 *       present), else 0.
 *   uint32_t __gj_virtio_is_avail_u  (alias)
 *   __libcgj_batch5239_marker = "libcgj-batch5239"
 *
 * Exclusive continuum CREATE-ONLY (5231-5240: virtio unique —
 * virtio_magic_u, virtio_ver1_u, virtio_status_ack_u,
 * virtio_status_driver_u, virtio_status_ok_u, virtio_status_fail_u,
 * virtio_q_align_u, virtio_desc_size_u, virtio_is_avail_u,
 * batch_id_5240). Unique gj_virtio_is_avail_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5239_marker[] = "libcgj-batch5239";

/* Available-flag / bit0 of a VirtIO flags word. */
#define B5239_AVAIL_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5239_is_avail(uint32_t u32Flags)
{
	if ((u32Flags & B5239_AVAIL_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_is_avail_u - 1 if VirtIO flags bit0 (available) is set.
 *
 * flags: VirtIO flags word (bit0 = available)
 *
 * Returns 1 when bit0 is set, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_virtio_is_avail_u(uint32_t u32Flags)
{
	(void)NULL;
	return b5239_is_avail(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_is_avail_u(uint32_t u32Flags)
    __attribute__((alias("gj_virtio_is_avail_u")));
