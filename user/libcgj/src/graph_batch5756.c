/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5756: audio device graph edge packing.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_edge_pack_5756(uint32_t src_id, uint32_t sink_id);
 *     - Pack a directed graph edge (source → sink) into 32 bits:
 *         bit0..15  = src_id  (clamped to 16 bits)
 *         bit16..31 = sink_id (clamped to 16 bits)
 *   uint32_t gj_adev_edge_src_5756(uint32_t packed);
 *   uint32_t gj_adev_edge_sink_5756(uint32_t packed);
 *   int gj_adev_edge_is_loop_5756(uint32_t packed);
 *     - 1 if src_id == sink_id (self-loop), else 0.
 *   uint32_t gj_batch_id_5756(void);
 *   __ aliases
 *   __libcgj_batch5756_marker = "libcgj-batch5756"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5756 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5756_marker[] = "libcgj-batch5756";

#define B5756_BATCH_ID  5756u
#define B5756_ID_MASK   0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5756_pack(uint32_t u32Src, uint32_t u32Sink)
{
	u32Src &= B5756_ID_MASK;
	u32Sink &= B5756_ID_MASK;
	return (u32Sink << 16) | u32Src;
}

static uint32_t
b5756_src(uint32_t u32P)
{
	return u32P & B5756_ID_MASK;
}

static uint32_t
b5756_sink(uint32_t u32P)
{
	return (u32P >> 16) & B5756_ID_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_edge_pack_5756 - pack directed source→sink edge ids.
 */
uint32_t
gj_adev_edge_pack_5756(uint32_t src_id, uint32_t sink_id)
{
	(void)NULL;
	return b5756_pack(src_id, sink_id);
}

uint32_t
gj_adev_edge_src_5756(uint32_t packed)
{
	return b5756_src(packed);
}

uint32_t
gj_adev_edge_sink_5756(uint32_t packed)
{
	return b5756_sink(packed);
}

int
gj_adev_edge_is_loop_5756(uint32_t packed)
{
	return (b5756_src(packed) == b5756_sink(packed)) ? 1 : 0;
}

uint32_t
gj_batch_id_5756(void)
{
	return B5756_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_edge_pack_5756(uint32_t src_id, uint32_t sink_id)
    __attribute__((alias("gj_adev_edge_pack_5756")));

uint32_t __gj_adev_edge_src_5756(uint32_t packed)
    __attribute__((alias("gj_adev_edge_src_5756")));

uint32_t __gj_adev_edge_sink_5756(uint32_t packed)
    __attribute__((alias("gj_adev_edge_sink_5756")));

int __gj_adev_edge_is_loop_5756(uint32_t packed)
    __attribute__((alias("gj_adev_edge_is_loop_5756")));

uint32_t __gj_batch_id_5756(void)
    __attribute__((alias("gj_batch_id_5756")));
