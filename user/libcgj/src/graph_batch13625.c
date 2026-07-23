/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13625: amdgpu ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdgpu_ok_u_13625(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       amdgpu DRM/kernel driver probe for the mesa soft continuum.
 *   uint32_t __gj_amdgpu_ok_u_13625  (alias)
 *   __libcgj_batch13625_marker = "libcgj-batch13625"
 *
 * Exclusive continuum CREATE-ONLY (13621-13630: mesa soft stubs —
 * all soft lamps →0; batch_id→13630 —
 * mesa_ok_u_13621, vulkan_ok_u_13622, radeonsi_ok_u_13623,
 * radv_ok_u_13624, amdgpu_ok_u_13625, aco_ok_u_13626, zink_ok_u_13627,
 * lavapipe_ok_u_13628, mesa_soft_ready_u_13629, batch_id_13630).
 * Unique surface only; no multi-def. Distinct from
 * gj_amdgpu_ok_u_13425, gj_amdgpu_ok_u_10525, gj_amdgpu_ok_u_10325,
 * gj_amdgpu_ok_u_10225 and sibling soft stubs in this wave. No parent
 * wires. No __int128. No amdgpu implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13625_marker[] = "libcgj-batch13625";

/* Soft amdgpu-ok lamp: always off (not a real runtime probe). */
#define B13625_AMDGPU_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13625_ok(void)
{
	return B13625_AMDGPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdgpu_ok_u_13625 - amdgpu ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open DRM nodes or
 * call libc. No parent wires.
 */
uint32_t
gj_amdgpu_ok_u_13625(void)
{
	(void)NULL;
	return b13625_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdgpu_ok_u_13625(void)
    __attribute__((alias("gj_amdgpu_ok_u_13625")));
