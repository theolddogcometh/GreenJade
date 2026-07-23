/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5760: audio device graph continuum id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5760(void);
 *     - Returns the compile-time graph batch number for this TU (5760).
 *   uint32_t gj_graph_milestone_5760(void);
 *     - Returns milestone revision 5760 (end of 5751-5760 wave).
 *   uint32_t gj_adev_graph_wave_5760(void);
 *     - Wave fingerprint: 0x16771680 (5751<<16 | 5760).
 *   uint32_t gj_adev_graph_wave_ready_5760(void);
 *     - Soft readiness constant 1 for audio-device-graph continuum.
 *   uint32_t __gj_batch_id_5760 / __gj_graph_milestone_5760
 *   uint32_t __gj_adev_graph_wave_5760 / __gj_adev_graph_wave_ready_5760
 *   __libcgj_batch5760_marker = "libcgj-batch5760"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs —
 * node_handle_5751, sink_rate_5752, src_rate_5753, latency_ms_5754,
 * frames_from_ms_5755, edge_pack_5756, period_frames_5757,
 * rate_ratio_5758, graph_depth_5759, batch_id_5760). Unique *_5760
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5760_marker[] = "libcgj-batch5760";

#define B5760_BATCH_ID  5760u
#define B5760_WAVE_LO   5751u
#define B5760_WAVE_HI   5760u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5760_id(void)
{
	return B5760_BATCH_ID;
}

static uint32_t
b5760_wave_fp(void)
{
	return (B5760_WAVE_LO << 16) | B5760_WAVE_HI;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_batch_id_5760(void)
{
	(void)NULL;
	return b5760_id();
}

uint32_t
gj_graph_milestone_5760(void)
{
	return b5760_id();
}

uint32_t
gj_adev_graph_wave_5760(void)
{
	return b5760_wave_fp();
}

uint32_t
gj_adev_graph_wave_ready_5760(void)
{
	return 1u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5760(void)
    __attribute__((alias("gj_batch_id_5760")));

uint32_t __gj_graph_milestone_5760(void)
    __attribute__((alias("gj_graph_milestone_5760")));

uint32_t __gj_adev_graph_wave_5760(void)
    __attribute__((alias("gj_adev_graph_wave_5760")));

uint32_t __gj_adev_graph_wave_ready_5760(void)
    __attribute__((alias("gj_adev_graph_wave_ready_5760")));
