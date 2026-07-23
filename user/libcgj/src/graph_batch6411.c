/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6411: OpenXR runtime API version soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_api_version_ok_6411(uint32_t major, uint32_t minor);
 *     - Soft OpenXR API version gate: major in [1, 15] and minor <= 99
 *       → 1; else 0. Integer-only stub; not a loader.
 *   uint32_t __gj_oxr_api_version_ok_6411  (alias)
 *   __libcgj_batch6411_marker = "libcgj-batch6411"
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

const char __libcgj_batch6411_marker[] = "libcgj-batch6411";

#define B6411_MAJOR_MIN  1u
#define B6411_MAJOR_MAX  15u
#define B6411_MINOR_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6411_api_ok(uint32_t u32Major, uint32_t u32Minor)
{
	if (u32Major < B6411_MAJOR_MIN || u32Major > B6411_MAJOR_MAX) {
		return 0u;
	}
	if (u32Minor > B6411_MINOR_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_api_version_ok_6411 - soft OpenXR API version validity.
 *
 * major: requested OpenXR major version
 * minor: requested OpenXR minor version
 *
 * Returns 1 when major in [1, 15] and minor <= 99, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_oxr_api_version_ok_6411(uint32_t major, uint32_t minor)
{
	(void)NULL;
	return b6411_api_ok(major, minor);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_api_version_ok_6411(uint32_t major, uint32_t minor)
    __attribute__((alias("gj_oxr_api_version_ok_6411")));
