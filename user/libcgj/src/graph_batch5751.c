/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5751: audio device graph node handle fabric.
 *
 * Surface (unique symbols):
 *   uint64_t gj_adev_node_handle_5751(uint32_t dev_id, uint32_t kind);
 *     - Pure local handle fabric (not a kernel/ALSA call):
 *       (batch_tag << 48) | (kind << 32) | dev_id. kind clamped 0..7
 *       (0 unknown, 1 sink, 2 source, 3 filter, 4 monitor, 5..7 reserved).
 *   uint32_t gj_adev_node_id_5751(uint64_t handle);
 *   uint32_t gj_adev_node_kind_5751(uint64_t handle);
 *   int gj_adev_node_valid_5751(uint64_t handle);
 *     - 1 if high tag nibble matches this batch fabric, else 0.
 *   uint32_t gj_batch_id_5751(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5751_marker = "libcgj-batch5751"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5751 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5751_marker[] = "libcgj-batch5751";

#define B5751_BATCH_ID  5751u
#define B5751_TAG       0x5751u
#define B5751_KIND_MAX  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5751_handle(uint32_t u32Dev, uint32_t u32Kind)
{
	if (u32Kind >= B5751_KIND_MAX) {
		u32Kind = 0u;
	}
	return ((uint64_t)B5751_TAG << 48) |
	       ((uint64_t)u32Kind << 32) |
	       (uint64_t)u32Dev;
}

static uint32_t
b5751_id(uint64_t u64H)
{
	return (uint32_t)(u64H & 0xffffffffull);
}

static uint32_t
b5751_kind(uint64_t u64H)
{
	return (uint32_t)((u64H >> 32) & 0xffffull);
}

static int
b5751_valid(uint64_t u64H)
{
	return (((u64H >> 48) & 0xffffull) == (uint64_t)B5751_TAG) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_node_handle_5751 - fabricate a local audio-device graph node
 * handle from device id and kind. Not an OS IPC call.
 */
uint64_t
gj_adev_node_handle_5751(uint32_t dev_id, uint32_t kind)
{
	(void)NULL;
	return b5751_handle(dev_id, kind);
}

uint32_t
gj_adev_node_id_5751(uint64_t handle)
{
	return b5751_id(handle);
}

uint32_t
gj_adev_node_kind_5751(uint64_t handle)
{
	return b5751_kind(handle);
}

int
gj_adev_node_valid_5751(uint64_t handle)
{
	return b5751_valid(handle);
}

uint32_t
gj_batch_id_5751(void)
{
	return B5751_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint64_t __gj_adev_node_handle_5751(uint32_t dev_id, uint32_t kind)
    __attribute__((alias("gj_adev_node_handle_5751")));

uint32_t __gj_adev_node_id_5751(uint64_t handle)
    __attribute__((alias("gj_adev_node_id_5751")));

uint32_t __gj_adev_node_kind_5751(uint64_t handle)
    __attribute__((alias("gj_adev_node_kind_5751")));

int __gj_adev_node_valid_5751(uint64_t handle)
    __attribute__((alias("gj_adev_node_valid_5751")));

uint32_t __gj_batch_id_5751(void)
    __attribute__((alias("gj_batch_id_5751")));
