/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5414: Vulkan instance enum / app-info stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_instance_ext_enumerate_stub_5414(void);
 *     - Returns 1 (extension enumerate soft success stub).
 *   uint32_t gj_vk_instance_destroy_stub_5414(void);
 *     - Returns 1 (destroy soft success stub).
 *   uint32_t gj_vk_app_info_ok_5414(uint32_t api_version);
 *     - Returns 1 if api_version is at least Vulkan 1.0 packed.
 *   uint32_t __gj_vk_instance_ext_enumerate_stub_5414  (alias)
 *   uint32_t __gj_vk_instance_destroy_stub_5414  (alias)
 *   uint32_t __gj_vk_app_info_ok_5414  (alias)
 *   __libcgj_batch5414_marker = "libcgj-batch5414"
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

const char __libcgj_batch5414_marker[] = "libcgj-batch5414";

/* VK_MAKE_VERSION(1, 0, 0) style packed floor. */
#define B5414_API_FLOOR  0x00400000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5414_app_ok(uint32_t uApi)
{
	return (uApi >= B5414_API_FLOOR) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_instance_ext_enumerate_stub_5414 - soft extension enum stub.
 *
 * Always returns 1. Product-path integer tag only.
 */
uint32_t
gj_vk_instance_ext_enumerate_stub_5414(void)
{
	(void)NULL;
	return 1u;
}

/*
 * gj_vk_instance_destroy_stub_5414 - soft vkDestroyInstance success stub.
 *
 * Always returns 1.
 */
uint32_t
gj_vk_instance_destroy_stub_5414(void)
{
	return 1u;
}

/*
 * gj_vk_app_info_ok_5414 - 1 if packed api_version meets Vulkan 1.0 floor.
 *
 * api_version: VkApplicationInfo::apiVersion style packed uint32.
 */
uint32_t
gj_vk_app_info_ok_5414(uint32_t uApiVersion)
{
	return b5414_app_ok(uApiVersion);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_instance_ext_enumerate_stub_5414(void)
    __attribute__((alias("gj_vk_instance_ext_enumerate_stub_5414")));

uint32_t __gj_vk_instance_destroy_stub_5414(void)
    __attribute__((alias("gj_vk_instance_destroy_stub_5414")));

uint32_t __gj_vk_app_info_ok_5414(uint32_t uApiVersion)
    __attribute__((alias("gj_vk_app_info_ok_5414")));
