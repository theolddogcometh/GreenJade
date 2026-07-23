/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6414: OpenXR session state pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_session_state_pack_6414(uint32_t state,
 *                                           uint32_t flags);
 *     - Soft session word: (state & 0xff) | ((flags & 0xffffff) << 8).
 *       Soft XR session states 0..255; flags low 24 bits only.
 *   uint32_t __gj_oxr_session_state_pack_6414  (alias)
 *   __libcgj_batch6414_marker = "libcgj-batch6414"
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

const char __libcgj_batch6414_marker[] = "libcgj-batch6414";

#define B6414_STATE_MASK  0x000000ffu
#define B6414_FLAGS_MASK  0x00ffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6414_state_pack(uint32_t u32State, uint32_t u32Flags)
{
	return (u32State & B6414_STATE_MASK) |
	    ((u32Flags & B6414_FLAGS_MASK) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_session_state_pack_6414 - soft OpenXR session state word.
 *
 * state: soft session state code (low 8 bits retained)
 * flags: soft session flags (low 24 bits retained, shifted to bits 8..31)
 *
 * Returns packed soft state. Pure integer. No parent wires.
 */
uint32_t
gj_oxr_session_state_pack_6414(uint32_t state, uint32_t flags)
{
	(void)NULL;
	return b6414_state_pack(state, flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_session_state_pack_6414(uint32_t state, uint32_t flags)
    __attribute__((alias("gj_oxr_session_state_pack_6414")));
