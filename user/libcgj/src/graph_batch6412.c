/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6412: OpenXR instance create soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_instance_create_ok_6412(uint32_t app_name_len,
 *                                           uint32_t create_flags);
 *     - Soft instance-create gate: app_name_len in [1, 128] and
 *       create_flags low 8 bits only (no reserved high bits) → 1;
 *       else 0.
 *   uint32_t __gj_oxr_instance_create_ok_6412  (alias)
 *   __libcgj_batch6412_marker = "libcgj-batch6412"
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

const char __libcgj_batch6412_marker[] = "libcgj-batch6412";

#define B6412_NAME_MIN    1u
#define B6412_NAME_MAX    128u
#define B6412_FLAGS_MASK  0x000000ffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6412_create_ok(uint32_t u32NameLen, uint32_t u32Flags)
{
	if (u32NameLen < B6412_NAME_MIN || u32NameLen > B6412_NAME_MAX) {
		return 0u;
	}
	if ((u32Flags & ~B6412_FLAGS_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_instance_create_ok_6412 - soft OpenXR instance create gate.
 *
 * app_name_len: application name length excluding NUL
 * create_flags: create flags (only low 8 bits permitted)
 *
 * Returns 1 when soft gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_oxr_instance_create_ok_6412(uint32_t app_name_len, uint32_t create_flags)
{
	(void)NULL;
	return b6412_create_ok(app_name_len, create_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_instance_create_ok_6412(uint32_t app_name_len,
    uint32_t create_flags)
    __attribute__((alias("gj_oxr_instance_create_ok_6412")));
