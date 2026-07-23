/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13424: radv ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_ok_u_13424(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       radv/Vulkan ICD probe for the mesa soft continuum.
 *   uint32_t __gj_radv_ok_u_13424  (alias)
 *   __libcgj_batch13424_marker = "libcgj-batch13424"
 *
 * Exclusive continuum CREATE-ONLY (13421-13430: mesa soft stubs —
 * all soft lamps →0; batch_id→13430 —
 * mesa_ok_u_13421, vulkan_ok_u_13422, radeonsi_ok_u_13423,
 * radv_ok_u_13424, amdgpu_ok_u_13425, aco_ok_u_13426, zink_ok_u_13427,
 * lavapipe_ok_u_13428, mesa_soft_ready_u_13429, batch_id_13430).
 * Unique surface only; no multi-def. Distinct from gj_radv_ok_u_13222,
 * gj_radv_ok_u_13022, gj_radv_ok_u_12822, gj_radv_ok_u_12622,
 * gj_radv_ok_u_12422, gj_radv_ok_u_12222, gj_radv_ok_u_12022,
 * gj_radv_ok_u_11922, gj_radv_ok_u_11722, gj_radv_ok_u_11522,
 * gj_radv_ok_u_11322, gj_radv_ok_u_11122, gj_radv_ok_u_10922,
 * gj_radv_ok_u_10722, gj_radv_ok_u_10524, gj_radv_ok_u_10324,
 * gj_radv_ok_u_10224, gj_radv_ok_u_10022 and sibling soft stubs in this
 * wave. No parent wires. No __int128. No RADV implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13424_marker[] = "libcgj-batch13424";

/* Soft radv-ok lamp: always off (not a real runtime probe). */
#define B13424_RADV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13424_ok(void)
{
	return B13424_RADV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_ok_u_13424 - radv ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not load radv, enumerate
 * Vulkan devices, or call libc. No parent wires.
 */
uint32_t
gj_radv_ok_u_13424(void)
{
	(void)NULL;
	return b13424_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_ok_u_13424(void)
    __attribute__((alias("gj_radv_ok_u_13424")));
