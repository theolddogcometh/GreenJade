/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8911: Vulkan API version 1.0 soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_api_version_1_0_id_8911(void);
 *     - Returns the packed Vulkan API version 1.0 value (0x00400000),
 *       matching VK_API_VERSION_1_0 / VK_MAKE_API_VERSION(0,1,0,0).
 *   uint32_t __gj_vk_api_version_1_0_id_8911  (alias)
 *   __libcgj_batch8911_marker = "libcgj-batch8911"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_api_version_1_0_id_8911 surface only; no multi-def.
 * Distinct from gj_vk_api_1_0_u (batch5361). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8911_marker[] = "libcgj-batch8911";

/* Vulkan VK_API_VERSION_1_0 = VK_MAKE_API_VERSION(0, 1, 0, 0). */
#define B8911_VK_API_1_0  ((uint32_t)0x00400000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8911_vk_api_1_0(void)
{
	return B8911_VK_API_1_0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_api_version_1_0_id_8911 - report packed Vulkan API version 1.0.
 *
 * Always returns 0x00400000. Soft compile-time graphics API id stub;
 * does not call the Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_api_version_1_0_id_8911(void)
{
	(void)NULL;
	return b8911_vk_api_1_0();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_api_version_1_0_id_8911(void)
    __attribute__((alias("gj_vk_api_version_1_0_id_8911")));
