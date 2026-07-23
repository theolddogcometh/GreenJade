/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6418: OpenXR action set priority clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_action_set_prio_6418(uint32_t priority);
 *     - Soft action-set priority clamp to [0, 100]. Values above 100
 *       saturate at 100.
 *   uint32_t __gj_oxr_action_set_prio_6418  (alias)
 *   __libcgj_batch6418_marker = "libcgj-batch6418"
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

const char __libcgj_batch6418_marker[] = "libcgj-batch6418";

#define B6418_PRIO_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6418_prio(uint32_t u32Prio)
{
	if (u32Prio > B6418_PRIO_MAX) {
		return B6418_PRIO_MAX;
	}
	return u32Prio;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_action_set_prio_6418 - soft OpenXR action set priority clamp.
 *
 * priority: requested action set priority (soft scale)
 *
 * Returns priority clamped to [0, 100]. Pure integer. No parent wires.
 */
uint32_t
gj_oxr_action_set_prio_6418(uint32_t priority)
{
	(void)NULL;
	return b6418_prio(priority);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_action_set_prio_6418(uint32_t priority)
    __attribute__((alias("gj_oxr_action_set_prio_6418")));
