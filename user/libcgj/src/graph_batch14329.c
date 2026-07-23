/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14329: Mesa soft continuum lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_soft_ready_u_14329(void);
 *     - Always returns 0. Soft continuum product tag for mesa/vulkan
 *       soft-path stubs; intentionally not a real Mesa/Vulkan ready
 *       claim.
 *   uint32_t __gj_mesa_soft_ready_u_14329  (alias)
 *   __libcgj_batch14329_marker = "libcgj-batch14329"
 *
 * Exclusive continuum CREATE-ONLY (14321-14330: mesa/vulkan soft path
 * stubs — mesa_ok_u_14321, vulkan_ok_u_14322, radeonsi_ok_u_14323,
 * radv_ok_u_14324, amdgpu_ok_u_14325, aco_ok_u_14326, zink_ok_u_14327,
 * lavapipe_ok_u_14328, mesa_soft_ready_u_14329, batch_id_14330). Unique
 * surface only; no multi-def. mesa/vulkan/radeonsi/radv/amdgpu/aco/
 * zink/lavapipe ok units remain 0. Distinct from gj_mesa_soft_ready_u_14129.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14329_marker[] = "libcgj-batch14329";

/* Soft continuum lamp: off (not a real Mesa/Vulkan ready claim). */
#define B14329_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14329_soft_ready(void)
{
	return B14329_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_soft_ready_u_14329 - Mesa soft continuum ready lamp.
 *
 * Always returns 0. Soft pure-data product tag for exclusive wave
 * 14321-14330. Does not claim mesa/vulkan/radeonsi/radv/amdgpu/aco/
 * zink/lavapipe ok and does not call libc. No parent wires.
 */
uint32_t
gj_mesa_soft_ready_u_14329(void)
{
	(void)NULL;
	return b14329_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_soft_ready_u_14329(void)
    __attribute__((alias("gj_mesa_soft_ready_u_14329")));
