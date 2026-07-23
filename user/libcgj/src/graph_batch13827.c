/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13827: zink ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_13827(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       zink GL-on-Vulkan probe for the mesa soft continuum.
 *   uint32_t __gj_zink_ok_u_13827  (alias)
 *   __libcgj_batch13827_marker = "libcgj-batch13827"
 *
 * Exclusive continuum CREATE-ONLY (13821-13830: mesa soft stubs —
 * all soft lamps →0; batch_id→13830 —
 * mesa_ok_u_13821, vulkan_ok_u_13822, radeonsi_ok_u_13823,
 * radv_ok_u_13824, amdgpu_ok_u_13825, aco_ok_u_13826, zink_ok_u_13827,
 * lavapipe_ok_u_13828, mesa_soft_ready_u_13829, batch_id_13830).
 * Unique surface only; no multi-def. Distinct from gj_zink_ok_u_13627,
 * gj_zink_ok_u_13427, gj_zink_ok_u_13226, gj_zink_ok_u_13026,
 * gj_zink_ok_u_12826, gj_zink_ok_u_12626, gj_zink_ok_u_12426,
 * gj_zink_ok_u_12226, gj_zink_ok_u_12026, gj_zink_ok_u_11926,
 * gj_zink_ok_u_11726, gj_zink_ok_u_11526, gj_zink_ok_u_11326,
 * gj_zink_ok_u_11126, gj_zink_ok_u_10926, gj_zink_ok_u_10726,
 * gj_zink_ok_u_10527, gj_zink_ok_u_10327, gj_zink_ok_u_10227,
 * gj_zink_ok_u_10026 and sibling soft stubs in this wave. No parent
 * wires. No __int128. No Zink implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13827_marker[] = "libcgj-batch13827";

/* Soft zink-ok lamp: always off (not a real runtime probe). */
#define B13827_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13827_ok(void)
{
	return B13827_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_13827 - zink ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not load zink GL-on-Vulkan
 * or call libc. No parent wires.
 */
uint32_t
gj_zink_ok_u_13827(void)
{
	(void)NULL;
	return b13827_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_13827(void)
    __attribute__((alias("gj_zink_ok_u_13827")));
