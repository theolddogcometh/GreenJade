/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13828: lavapipe ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lavapipe_ok_u_13828(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       lavapipe software Vulkan ICD probe for the mesa soft continuum.
 *   uint32_t __gj_lavapipe_ok_u_13828  (alias)
 *   __libcgj_batch13828_marker = "libcgj-batch13828"
 *
 * Exclusive continuum CREATE-ONLY (13821-13830: mesa soft stubs —
 * all soft lamps →0; batch_id→13830 —
 * mesa_ok_u_13821, vulkan_ok_u_13822, radeonsi_ok_u_13823,
 * radv_ok_u_13824, amdgpu_ok_u_13825, aco_ok_u_13826, zink_ok_u_13827,
 * lavapipe_ok_u_13828, mesa_soft_ready_u_13829, batch_id_13830).
 * Unique surface only; no multi-def. Distinct from
 * gj_lavapipe_ok_u_13628, gj_lavapipe_ok_u_13428, gj_lavapipe_ok_u_13225,
 * gj_lavapipe_ok_u_13025, gj_lavapipe_ok_u_12825, gj_lavapipe_ok_u_12625,
 * gj_lavapipe_ok_u_12425, gj_lavapipe_ok_u_12225, gj_lavapipe_ok_u_12025,
 * gj_lavapipe_ok_u_11925, gj_lavapipe_ok_u_11725, gj_lavapipe_ok_u_11525,
 * gj_lavapipe_ok_u_11325, gj_lavapipe_ok_u_11125, gj_lavapipe_ok_u_10925,
 * gj_lavapipe_ok_u_10725, gj_lavapipe_ok_u_10528, gj_lavapipe_ok_u_10328,
 * gj_lavapipe_ok_u_10228, gj_lavapipe_ok_u_10025 and sibling soft stubs
 * in this wave. No parent wires. No __int128. No lavapipe
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13828_marker[] = "libcgj-batch13828";

/* Soft lavapipe-ok lamp: always off (not a real runtime probe). */
#define B13828_LAVAPIPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13828_ok(void)
{
	return B13828_LAVAPIPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lavapipe_ok_u_13828 - lavapipe ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not load lavapipe ICD
 * or call libc. No parent wires.
 */
uint32_t
gj_lavapipe_ok_u_13828(void)
{
	(void)NULL;
	return b13828_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lavapipe_ok_u_13828(void)
    __attribute__((alias("gj_lavapipe_ok_u_13828")));
