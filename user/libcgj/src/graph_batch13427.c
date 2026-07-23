/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13427: zink ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_13427(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       zink GL-on-Vulkan probe for the mesa soft continuum.
 *   uint32_t __gj_zink_ok_u_13427  (alias)
 *   __libcgj_batch13427_marker = "libcgj-batch13427"
 *
 * Exclusive continuum CREATE-ONLY (13421-13430: mesa soft stubs —
 * all soft lamps →0; batch_id→13430 —
 * mesa_ok_u_13421, vulkan_ok_u_13422, radeonsi_ok_u_13423,
 * radv_ok_u_13424, amdgpu_ok_u_13425, aco_ok_u_13426, zink_ok_u_13427,
 * lavapipe_ok_u_13428, mesa_soft_ready_u_13429, batch_id_13430).
 * Unique surface only; no multi-def. Distinct from gj_zink_ok_u_13226,
 * gj_zink_ok_u_13026, gj_zink_ok_u_12826, gj_zink_ok_u_12626,
 * gj_zink_ok_u_12426, gj_zink_ok_u_12226, gj_zink_ok_u_12026,
 * gj_zink_ok_u_11926, gj_zink_ok_u_11726, gj_zink_ok_u_11526,
 * gj_zink_ok_u_11326, gj_zink_ok_u_11126, gj_zink_ok_u_10926,
 * gj_zink_ok_u_10726, gj_zink_ok_u_10527, gj_zink_ok_u_10327,
 * gj_zink_ok_u_10227, gj_zink_ok_u_10026 and sibling soft stubs in this
 * wave. No parent wires. No __int128. No Zink implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13427_marker[] = "libcgj-batch13427";

/* Soft zink-ok lamp: always off (not a real runtime probe). */
#define B13427_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13427_ok(void)
{
	return B13427_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_13427 - zink ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not load zink or call
 * libc. No parent wires.
 */
uint32_t
gj_zink_ok_u_13427(void)
{
	(void)NULL;
	return b13427_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_13427(void)
    __attribute__((alias("gj_zink_ok_u_13427")));
