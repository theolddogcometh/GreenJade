/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5413: Vulkan instance create stubs (Deck).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_instance_create_stub_5413(void);
 *     - Returns 1 (instance create soft success stub).
 *   uint32_t gj_vk_instance_api_version_5413(void);
 *     - Returns packed instance API version product hint (1.2).
 *   uint32_t gj_vk_instance_ext_count_hint_5413(void);
 *     - Returns soft enabled-extension count hint for Deck path.
 *   uint32_t __gj_vk_instance_create_stub_5413  (alias)
 *   uint32_t __gj_vk_instance_api_version_5413  (alias)
 *   uint32_t __gj_vk_instance_ext_count_hint_5413  (alias)
 *   __libcgj_batch5413_marker = "libcgj-batch5413"
 *
 * Vulkan ICD / instance / device / queue / swapchain Deck exclusive
 * CREATE-ONLY wave (5411-5420). Unique batch-suffixed symbols only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5413_marker[] = "libcgj-batch5413";

/* VK_MAKE_VERSION(1, 2, 0) style packed. */
#define B5413_API_VER   0x00402000u
#define B5413_EXT_HINT  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5413_create_ok(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_instance_create_stub_5413 - soft vkCreateInstance success stub.
 *
 * Always returns 1. No real loader call; product-path integer tag.
 */
uint32_t
gj_vk_instance_create_stub_5413(void)
{
	(void)NULL;
	return b5413_create_ok();
}

/*
 * gj_vk_instance_api_version_5413 - instance API version product hint.
 *
 * Returns packed Vulkan 1.2.0 style constant (0x00402000).
 */
uint32_t
gj_vk_instance_api_version_5413(void)
{
	return B5413_API_VER;
}

/*
 * gj_vk_instance_ext_count_hint_5413 - soft enabled extension count.
 *
 * Always returns 4 (typical surface/wsi set size product hint).
 */
uint32_t
gj_vk_instance_ext_count_hint_5413(void)
{
	return B5413_EXT_HINT;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_instance_create_stub_5413(void)
    __attribute__((alias("gj_vk_instance_create_stub_5413")));

uint32_t __gj_vk_instance_api_version_5413(void)
    __attribute__((alias("gj_vk_instance_api_version_5413")));

uint32_t __gj_vk_instance_ext_count_hint_5413(void)
    __attribute__((alias("gj_vk_instance_ext_count_hint_5413")));
