/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5852: VK_KHR_swapchain extension hash stub
 * (Steam Deck present path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_swapchain_5852(void);
 *     - Returns the FNV-1a 32 digest of "VK_KHR_swapchain"
 *       (0x2fab4bbd). Soft compile-time constant; not a live enum.
 *   uint32_t gj_vk_ext_match_swapchain_5852(uint32_t hash);
 *     - Return 1 if hash equals the swapchain extension digest, else 0.
 *   uint32_t __gj_vk_ext_hash_swapchain_5852  (alias)
 *   uint32_t __gj_vk_ext_match_swapchain_5852  (alias)
 *   __libcgj_batch5852_marker = "libcgj-batch5852"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5852 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5852_marker[] = "libcgj-batch5852";

/* FNV-1a 32 of "VK_KHR_swapchain" (verified offline; public-domain FNV). */
#define B5852_HASH_SWAPCHAIN  0x2fab4bbdu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5852_hash(void)
{
	return B5852_HASH_SWAPCHAIN;
}

static uint32_t
b5852_match(uint32_t u32Hash)
{
	return (u32Hash == B5852_HASH_SWAPCHAIN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_swapchain_5852 - soft VK_KHR_swapchain name hash.
 *
 * Always returns 0x2fab4bbd. Integer-only Deck present-path stub; does
 * not create a VkSwapchainKHR. No parent wires.
 */
uint32_t
gj_vk_ext_hash_swapchain_5852(void)
{
	(void)NULL;
	return b5852_hash();
}

/*
 * gj_vk_ext_match_swapchain_5852 - test hash against swapchain digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_swapchain_5852(uint32_t hash)
{
	return b5852_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_swapchain_5852(void)
    __attribute__((alias("gj_vk_ext_hash_swapchain_5852")));

uint32_t __gj_vk_ext_match_swapchain_5852(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_swapchain_5852")));
