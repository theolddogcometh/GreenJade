/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10325: amdgpu ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdgpu_ok_u_10325(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       amdgpu runtime probe for the bar3-related continuum.
 *   uint32_t __gj_amdgpu_ok_u_10325  (alias)
 *   __libcgj_batch10325_marker = "libcgj-batch10325"
 *
 * Exclusive continuum CREATE-ONLY (10321-10330: mesa/vulkan soft
 * id stubs — mesa_ok_u_10321, vulkan_ok_u_10322,
 * radeonsi_ok_u_10323, radv_ok_u_10324, amdgpu_ok_u_10325,
 * aco_ok_u_10326, zink_ok_u_10327, lavapipe_ok_u_10328,
 * mesa_soft_ready_u_10329, batch_id_10330). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10325_marker[] = "libcgj-batch10325";

/* Soft amdgpu-ok lamp: always off (not a real runtime probe). */
#define B10325_AMDGPU_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10325_ok(void)
{
	return B10325_AMDGPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdgpu_ok_u_10325 - amdgpu ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe amdgpu
 * kernel/DRM presence or call libc. No parent wires.
 */
uint32_t
gj_amdgpu_ok_u_10325(void)
{
	(void)NULL;
	return b10325_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdgpu_ok_u_10325(void)
    __attribute__((alias("gj_amdgpu_ok_u_10325")));
