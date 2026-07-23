/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6419: OpenXR frame budget ms soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_frame_budget_ms_6419(uint32_t hz,
 *                                        uint32_t slack_ms);
 *     - Soft per-frame budget ms: (1000 / hz) + slack_ms when hz in
 *       [30, 240]; else 0. Caps sum at 100 ms.
 *   uint32_t __gj_oxr_frame_budget_ms_6419  (alias)
 *   __libcgj_batch6419_marker = "libcgj-batch6419"
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

const char __libcgj_batch6419_marker[] = "libcgj-batch6419";

#define B6419_HZ_MIN     30u
#define B6419_HZ_MAX     240u
#define B6419_MS_PER_S   1000u
#define B6419_BUDGET_CAP 100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6419_budget(uint32_t u32Hz, uint32_t u32Slack)
{
	uint32_t u32Period;
	uint32_t u32Sum;

	if (u32Hz < B6419_HZ_MIN || u32Hz > B6419_HZ_MAX) {
		return 0u;
	}
	u32Period = B6419_MS_PER_S / u32Hz;
	if (u32Slack > (B6419_BUDGET_CAP - u32Period)) {
		return B6419_BUDGET_CAP;
	}
	u32Sum = u32Period + u32Slack;
	if (u32Sum > B6419_BUDGET_CAP) {
		return B6419_BUDGET_CAP;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_frame_budget_ms_6419 - soft OpenXR per-frame time budget ms.
 *
 * hz:       target display refresh rate [30, 240]
 * slack_ms: extra soft margin milliseconds
 *
 * Returns (1000/hz)+slack capped at 100, or 0 if hz out of range.
 * Pure integer. No parent wires.
 */
uint32_t
gj_oxr_frame_budget_ms_6419(uint32_t hz, uint32_t slack_ms)
{
	(void)NULL;
	return b6419_budget(hz, slack_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_frame_budget_ms_6419(uint32_t hz, uint32_t slack_ms)
    __attribute__((alias("gj_oxr_frame_budget_ms_6419")));
