/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6417: OpenXR space location flags fold stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oxr_space_loc_flags_6417(uint32_t pos_valid,
 *                                        uint32_t ori_valid);
 *     - Soft location flags fold: bit0 = position valid (non-zero),
 *       bit1 = orientation valid (non-zero). Returns flags in [0, 3].
 *   uint32_t __gj_oxr_space_loc_flags_6417  (alias)
 *   __libcgj_batch6417_marker = "libcgj-batch6417"
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

const char __libcgj_batch6417_marker[] = "libcgj-batch6417";

#define B6417_POS_BIT  0x1u
#define B6417_ORI_BIT  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6417_loc_flags(uint32_t u32Pos, uint32_t u32Ori)
{
	uint32_t u32Flags;

	u32Flags = 0u;
	if (u32Pos != 0u) {
		u32Flags |= B6417_POS_BIT;
	}
	if (u32Ori != 0u) {
		u32Flags |= B6417_ORI_BIT;
	}
	return u32Flags;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oxr_space_loc_flags_6417 - soft OpenXR space location flags fold.
 *
 * pos_valid: non-zero when position tracking is soft-valid
 * ori_valid: non-zero when orientation tracking is soft-valid
 *
 * Returns bit0 (pos) | bit1 (ori). Pure integer. No parent wires.
 */
uint32_t
gj_oxr_space_loc_flags_6417(uint32_t pos_valid, uint32_t ori_valid)
{
	(void)NULL;
	return b6417_loc_flags(pos_valid, ori_valid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oxr_space_loc_flags_6417(uint32_t pos_valid,
    uint32_t ori_valid)
    __attribute__((alias("gj_oxr_space_loc_flags_6417")));
