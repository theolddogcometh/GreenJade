/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5415: Vulkan physical device enum (Deck GPU).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_phys_dev_enum_stub_5415(void);
 *     - Returns 1 (physical device enumerate soft success stub).
 *   uint32_t gj_vk_phys_dev_count_hint_5415(void);
 *     - Returns soft physical device count (always 1 for Deck iGPU).
 *   uint32_t gj_vk_phys_dev_deck_gpu_p_5415(uint32_t vendor_id);
 *     - Returns 1 if vendor_id matches AMD Deck iGPU product hint.
 *   uint32_t __gj_vk_phys_dev_enum_stub_5415  (alias)
 *   uint32_t __gj_vk_phys_dev_count_hint_5415  (alias)
 *   uint32_t __gj_vk_phys_dev_deck_gpu_p_5415  (alias)
 *   __libcgj_batch5415_marker = "libcgj-batch5415"
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

const char __libcgj_batch5415_marker[] = "libcgj-batch5415";

/* PCI vendor AMD (Deck Van Gogh / Sephiroth class). Soft product id. */
#define B5415_VENDOR_AMD  0x1002u
#define B5415_DEV_COUNT   1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5415_deck_gpu_p(uint32_t uVendor)
{
	return (uVendor == B5415_VENDOR_AMD) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_phys_dev_enum_stub_5415 - soft physical device enumerate stub.
 *
 * Always returns 1. Product-path integer tag only.
 */
uint32_t
gj_vk_phys_dev_enum_stub_5415(void)
{
	(void)NULL;
	return 1u;
}

/*
 * gj_vk_phys_dev_count_hint_5415 - soft phys-dev count for Deck iGPU.
 *
 * Always returns 1.
 */
uint32_t
gj_vk_phys_dev_count_hint_5415(void)
{
	return B5415_DEV_COUNT;
}

/*
 * gj_vk_phys_dev_deck_gpu_p_5415 - 1 if vendor_id is AMD (0x1002).
 *
 * vendor_id: VkPhysicalDeviceProperties::vendorID style PCI vendor.
 */
uint32_t
gj_vk_phys_dev_deck_gpu_p_5415(uint32_t uVendorId)
{
	return b5415_deck_gpu_p(uVendorId);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_phys_dev_enum_stub_5415(void)
    __attribute__((alias("gj_vk_phys_dev_enum_stub_5415")));

uint32_t __gj_vk_phys_dev_count_hint_5415(void)
    __attribute__((alias("gj_vk_phys_dev_count_hint_5415")));

uint32_t __gj_vk_phys_dev_deck_gpu_p_5415(uint32_t uVendorId)
    __attribute__((alias("gj_vk_phys_dev_deck_gpu_p_5415")));
