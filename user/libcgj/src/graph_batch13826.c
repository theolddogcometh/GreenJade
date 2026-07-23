/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13826: aco ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aco_ok_u_13826(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ACO (AMD compiler) probe for the mesa soft continuum.
 *   uint32_t __gj_aco_ok_u_13826  (alias)
 *   __libcgj_batch13826_marker = "libcgj-batch13826"
 *
 * Exclusive continuum CREATE-ONLY (13821-13830: mesa soft stubs —
 * all soft lamps →0; batch_id→13830 —
 * mesa_ok_u_13821, vulkan_ok_u_13822, radeonsi_ok_u_13823,
 * radv_ok_u_13824, amdgpu_ok_u_13825, aco_ok_u_13826, zink_ok_u_13827,
 * lavapipe_ok_u_13828, mesa_soft_ready_u_13829, batch_id_13830).
 * Unique surface only; no multi-def. Distinct from gj_aco_ok_u_13626,
 * gj_aco_ok_u_13426, gj_aco_ok_u_10526, gj_aco_ok_u_10326,
 * gj_aco_ok_u_10226 and sibling soft stubs in this wave. No parent
 * wires. No __int128. No ACO implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13826_marker[] = "libcgj-batch13826";

/* Soft aco-ok lamp: always off (not a real runtime probe). */
#define B13826_ACO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13826_ok(void)
{
	return B13826_ACO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aco_ok_u_13826 - aco ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not exercise the ACO
 * compiler path or call libc. No parent wires.
 */
uint32_t
gj_aco_ok_u_13826(void)
{
	(void)NULL;
	return b13826_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aco_ok_u_13826(void)
    __attribute__((alias("gj_aco_ok_u_13826")));
