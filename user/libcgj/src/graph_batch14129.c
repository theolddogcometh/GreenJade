/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14129: Mesa soft continuum lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_soft_ready_u_14129(void);
 *     - Always returns 0. Soft continuum product tag for mesa/vulkan
 *       soft-path stubs; intentionally not a real Mesa/Vulkan ready
 *       claim.
 *   uint32_t __gj_mesa_soft_ready_u_14129  (alias)
 *   __libcgj_batch14129_marker = "libcgj-batch14129"
 *
 * Exclusive continuum CREATE-ONLY (14121-14130: mesa/vulkan soft path
 * stubs — mesa_ok_u_14121, vulkan_ok_u_14122, radeonsi_ok_u_14123,
 * radv_ok_u_14124, amdgpu_ok_u_14125, aco_ok_u_14126, zink_ok_u_14127,
 * lavapipe_ok_u_14128, mesa_soft_ready_u_14129, batch_id_14130). Unique
 * surface only; no multi-def. mesa/vulkan/radeonsi/radv/amdgpu/aco/
 * zink/lavapipe ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14129_marker[] = "libcgj-batch14129";

/* Soft continuum lamp: off (not a real Mesa/Vulkan ready claim). */
#define B14129_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14129_soft_ready(void)
{
	return B14129_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_soft_ready_u_14129 - Mesa soft continuum ready lamp.
 *
 * Always returns 0. Soft pure-data product tag for exclusive wave
 * 14121-14130. Does not claim mesa/vulkan/radeonsi/radv/amdgpu/aco/
 * zink/lavapipe ok and does not call libc. No parent wires.
 */
uint32_t
gj_mesa_soft_ready_u_14129(void)
{
	(void)NULL;
	return b14129_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_soft_ready_u_14129(void)
    __attribute__((alias("gj_mesa_soft_ready_u_14129")));
