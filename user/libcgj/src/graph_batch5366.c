/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5366: test a Vulkan-style result for success.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_is_success_u(uint32_t r);
 *     - Return 1 if r equals VK_SUCCESS (0), else 0. Integer-only
 *       success probe for freestanding Vulkan result codes.
 *   uint32_t __gj_vk_is_success_u  (alias)
 *   __libcgj_batch5366_marker = "libcgj-batch5366"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Unique gj_vk_is_success_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5366_marker[] = "libcgj-batch5366";

/* Vulkan VK_SUCCESS result code (zero). */
#define B5366_VK_SUCCESS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5366_is_success(uint32_t u32R)
{
	if (u32R == B5366_VK_SUCCESS) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_is_success_u - 1 if r is VK_SUCCESS (0), else 0.
 *
 * r: Vulkan-style result code (integer bit pattern)
 *
 * Returns 1 when r == 0, else 0. Does not call the Vulkan loader or
 * libc. No parent wires.
 */
uint32_t
gj_vk_is_success_u(uint32_t u32R)
{
	(void)NULL;
	return b5366_is_success(u32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_is_success_u(uint32_t u32R)
    __attribute__((alias("gj_vk_is_success_u")));
