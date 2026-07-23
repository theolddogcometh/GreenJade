/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5560: controller / Steam Input continuum id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5560(void);
 *     - Returns the compile-time graph batch number for this TU (5560).
 *   uint32_t gj_graph_milestone_5560(void);
 *     - Returns milestone revision 5560 (end of 5551-5560 wave).
 *   uint32_t gj_steam_input_wave_5560(void);
 *     - Wave fingerprint: 0x15510560 (5551<<16 | 5560).
 *   uint32_t gj_ctrl_layout_wave_ready_5560(void);
 *     - Soft readiness constant 1 for controller-layout continuum.
 *   uint32_t __gj_batch_id_5560 / __gj_graph_milestone_5560
 *   uint32_t __gj_steam_input_wave_5560 / __gj_ctrl_layout_wave_ready_5560
 *   __libcgj_batch5560_marker = "libcgj-batch5560"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs —
 * glyph_code_5551, layout_pack_5552, gyro_bias_5553, gyro_scale_5554,
 * trackpad_norm_5555, trackpad_zone_5556, action_set_5557,
 * digital_pack_5558, analog_dz_5559, batch_id_5560). Unique *_5560
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5560_marker[] = "libcgj-batch5560";

#define B5560_BATCH_ID  5560u
#define B5560_WAVE_LO   5551u
#define B5560_WAVE_HI   5560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5560_id(void)
{
	return B5560_BATCH_ID;
}

static uint32_t
b5560_wave_fp(void)
{
	return (B5560_WAVE_LO << 16) | B5560_WAVE_HI;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_batch_id_5560(void)
{
	(void)NULL;
	return b5560_id();
}

uint32_t
gj_graph_milestone_5560(void)
{
	return b5560_id();
}

uint32_t
gj_steam_input_wave_5560(void)
{
	return b5560_wave_fp();
}

uint32_t
gj_ctrl_layout_wave_ready_5560(void)
{
	return 1u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5560(void)
    __attribute__((alias("gj_batch_id_5560")));

uint32_t __gj_graph_milestone_5560(void)
    __attribute__((alias("gj_graph_milestone_5560")));

uint32_t __gj_steam_input_wave_5560(void)
    __attribute__((alias("gj_steam_input_wave_5560")));

uint32_t __gj_ctrl_layout_wave_ready_5560(void)
    __attribute__((alias("gj_ctrl_layout_wave_ready_5560")));
