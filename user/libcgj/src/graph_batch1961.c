/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1961: Vulkan format depth/stencil soft hint.
 *
 * Surface (unique symbols):
 *   int gj_vk_format_is_depth_hint(uint32_t fmt);
 *     — Return 1 if fmt matches a core VkFormat depth/stencil value
 *       (D16, X8_D24, D32, S8, D16_S8, D24_S8, D32_S8), else 0.
 *       Soft enum-table hint only; does not query device support.
 *   int __gj_vk_format_is_depth_hint  (alias)
 *   __libcgj_batch1961_marker = "libcgj-batch1961"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1961_marker[] = "libcgj-batch1961";

/*
 * Core VkFormat depth/stencil values (Vulkan 1.0 enum table):
 *   VK_FORMAT_D16_UNORM            = 124
 *   VK_FORMAT_X8_D24_UNORM_PACK32  = 125
 *   VK_FORMAT_D32_SFLOAT           = 126
 *   VK_FORMAT_S8_UINT              = 127
 *   VK_FORMAT_D16_UNORM_S8_UINT    = 128
 *   VK_FORMAT_D24_UNORM_S8_UINT    = 129
 *   VK_FORMAT_D32_SFLOAT_S8_UINT   = 130
 */
#define B1961_DEPTH_LO  124u
#define B1961_DEPTH_HI  130u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if fmt is in the core depth/stencil VkFormat range. */
static int
b1961_is_depth(uint32_t uFmt)
{
	return (uFmt >= B1961_DEPTH_LO && uFmt <= B1961_DEPTH_HI) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_format_is_depth_hint — 1 if fmt is a core depth/stencil VkFormat.
 *
 * fmt: VkFormat as uint32_t
 * Returns 1 for 124..130 inclusive, else 0.
 */
int
gj_vk_format_is_depth_hint(uint32_t uFmt)
{
	(void)NULL;
	return b1961_is_depth(uFmt);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vk_format_is_depth_hint(uint32_t uFmt)
    __attribute__((alias("gj_vk_format_is_depth_hint")));
