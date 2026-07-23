/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6413: OpenXR system id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_system_id_ok_6413(uint32_t system_id);
 *     - Soft system-id check: 0 → 0; reserved all-bits-one
 *       (0xffffffff) → 0; else 1 when id in [1, 0xfffffffe].
 *   uint32_t __gj_oxr_system_id_ok_6413  (alias)
 *   __libcgj_batch6413_marker = "libcgj-batch6413"
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

const char __libcgj_batch6413_marker[] = "libcgj-batch6413";

#define B6413_ID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6413_sys_ok(uint32_t u32Id)
{
	if (u32Id == 0u || u32Id > B6413_ID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_system_id_ok_6413 - soft OpenXR system id validity.
 *
 * system_id: opaque 32-bit soft system handle (truncated XR system id)
 *
 * Returns 1 when id is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_oxr_system_id_ok_6413(uint32_t system_id)
{
	(void)NULL;
	return b6413_sys_ok(system_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_system_id_ok_6413(uint32_t system_id)
    __attribute__((alias("gj_oxr_system_id_ok_6413")));
