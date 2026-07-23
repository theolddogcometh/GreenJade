/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14325: amdgpu ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdgpu_ok_u_14325(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       amdgpu DRM/kernel probe for the mesa/vulkan continuum.
 *   uint32_t __gj_amdgpu_ok_u_14325  (alias)
 *   __libcgj_batch14325_marker = "libcgj-batch14325"
 *
 * Exclusive continuum CREATE-ONLY (14321-14330: mesa/vulkan soft path
 * stubs — mesa_ok_u_14321, vulkan_ok_u_14322, radeonsi_ok_u_14323,
 * radv_ok_u_14324, amdgpu_ok_u_14325, aco_ok_u_14326, zink_ok_u_14327,
 * lavapipe_ok_u_14328, mesa_soft_ready_u_14329, batch_id_14330). Unique
 * surface only; no multi-def. Distinct from prior amdgpu soft tags
 * (14125/…). No parent wires. No __int128. No amdgpu implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14325_marker[] = "libcgj-batch14325";

/* Soft amdgpu-ok lamp: always off (not a real amdgpu probe). */
#define B14325_AMDGPU_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14325_amdgpu_ok(void)
{
	return B14325_AMDGPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdgpu_ok_u_14325 - amdgpu ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DRM/amdgpu
 * or call libc. No parent wires.
 */
uint32_t
gj_amdgpu_ok_u_14325(void)
{
	(void)NULL;
	return b14325_amdgpu_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdgpu_ok_u_14325(void)
    __attribute__((alias("gj_amdgpu_ok_u_14325")));
