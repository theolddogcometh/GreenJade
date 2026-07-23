/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6415: OpenXR view config soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_view_config_ok_6415(uint32_t view_type,
 *                                       uint32_t view_count);
 *     - Soft view-config gate: view_type in {1 mono, 2 stereo} and
 *       view_count in [1, 4] → 1; else 0.
 *   uint32_t __gj_oxr_view_config_ok_6415  (alias)
 *   __libcgj_batch6415_marker = "libcgj-batch6415"
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

const char __libcgj_batch6415_marker[] = "libcgj-batch6415";

#define B6415_TYPE_MONO    1u
#define B6415_TYPE_STEREO  2u
#define B6415_COUNT_MIN    1u
#define B6415_COUNT_MAX    4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6415_view_ok(uint32_t u32Type, uint32_t u32Count)
{
	if (u32Type != B6415_TYPE_MONO && u32Type != B6415_TYPE_STEREO) {
		return 0u;
	}
	if (u32Count < B6415_COUNT_MIN || u32Count > B6415_COUNT_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_view_config_ok_6415 - soft OpenXR view configuration gate.
 *
 * view_type:  1 = mono, 2 = stereo (other values rejected)
 * view_count: number of views [1, 4]
 *
 * Returns 1 when soft gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_oxr_view_config_ok_6415(uint32_t view_type, uint32_t view_count)
{
	(void)NULL;
	return b6415_view_ok(view_type, view_count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_view_config_ok_6415(uint32_t view_type,
    uint32_t view_count)
    __attribute__((alias("gj_oxr_view_config_ok_6415")));
