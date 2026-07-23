/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6416: OpenXR swapchain extent soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_swapchain_extent_ok_6416(uint32_t width,
 *                                            uint32_t height);
 *     - Soft swapchain extent gate: width and height each in
 *       [64, 8192] and both even → 1; else 0.
 *   uint32_t __gj_oxr_swapchain_extent_ok_6416  (alias)
 *   __libcgj_batch6416_marker = "libcgj-batch6416"
 *
 * Exclusive continuum CREATE-ONLY (6411-6420: OpenXR runtime stubs —
 * api_version_ok_6411, instance_create_ok_6412, system_id_ok_6413,
 * session_state_pack_6414, view_config_ok_6415, swapchain_extent_ok_6416,
 * space_loc_flags_6417, action_set_prio_6418, frame_budget_ms_6419,
 * runtime_ready_6420). Unique surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6416_marker[] = "libcgj-batch6416";

#define B6416_EXT_MIN  64u
#define B6416_EXT_MAX  8192u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6416_extent_ok(uint32_t u32W, uint32_t u32H)
{
	if (u32W < B6416_EXT_MIN || u32W > B6416_EXT_MAX) {
		return 0u;
	}
	if (u32H < B6416_EXT_MIN || u32H > B6416_EXT_MAX) {
		return 0u;
	}
	if ((u32W & 1u) != 0u || (u32H & 1u) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_swapchain_extent_ok_6416 - soft OpenXR swapchain extent gate.
 *
 * width:  swapchain image width in pixels
 * height: swapchain image height in pixels
 *
 * Returns 1 when both dims in [64, 8192] and even, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_oxr_swapchain_extent_ok_6416(uint32_t width, uint32_t height)
{
	(void)NULL;
	return b6416_extent_ok(width, height);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_swapchain_extent_ok_6416(uint32_t width, uint32_t height)
    __attribute__((alias("gj_oxr_swapchain_extent_ok_6416")));
