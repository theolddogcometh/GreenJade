/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13422: vulkan ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vulkan_ok_u_13422(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vulkan runtime probe for the mesa soft continuum.
 *   uint32_t __gj_vulkan_ok_u_13422  (alias)
 *   __libcgj_batch13422_marker = "libcgj-batch13422"
 *
 * Exclusive continuum CREATE-ONLY (13421-13430: mesa soft stubs —
 * all soft lamps →0; batch_id→13430 —
 * mesa_ok_u_13421, vulkan_ok_u_13422, radeonsi_ok_u_13423,
 * radv_ok_u_13424, amdgpu_ok_u_13425, aco_ok_u_13426, zink_ok_u_13427,
 * lavapipe_ok_u_13428, mesa_soft_ready_u_13429, batch_id_13430).
 * Unique surface only; no multi-def. Distinct from
 * gj_vulkan_ok_u_10522, gj_vulkan_ok_u_10322, gj_vulkan_ok_u_10222 and
 * gj_mesa_ok_u_13421. No parent wires. No __int128. No Vulkan
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13422_marker[] = "libcgj-batch13422";

/* Soft vulkan-ok lamp: always off (not a real runtime probe). */
#define B13422_VULKAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13422_ok(void)
{
	return B13422_VULKAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vulkan_ok_u_13422 - vulkan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe vulkan
 * presence, load ICDs, or call libc. No parent wires.
 */
uint32_t
gj_vulkan_ok_u_13422(void)
{
	(void)NULL;
	return b13422_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vulkan_ok_u_13422(void)
    __attribute__((alias("gj_vulkan_ok_u_13422")));
