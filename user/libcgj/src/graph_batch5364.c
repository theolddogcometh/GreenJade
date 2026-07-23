/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5364: Vulkan API version 1.3 packed constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_api_1_3_u(void);
 *     - Returns the packed Vulkan API version 1.3 value (0x00403000),
 *       matching VK_API_VERSION_1_3 / VK_MAKE_API_VERSION(0,1,3,0).
 *   uint32_t __gj_vk_api_1_3_u  (alias)
 *   __libcgj_batch5364_marker = "libcgj-batch5364"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Unique gj_vk_api_1_3_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5364_marker[] = "libcgj-batch5364";

/* Vulkan VK_API_VERSION_1_3 = VK_MAKE_API_VERSION(0, 1, 3, 0). */
#define B5364_VK_API_1_3  ((uint32_t)0x00403000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5364_vk_api_1_3(void)
{
	return B5364_VK_API_1_3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_api_1_3_u - report packed Vulkan API version 1.3.
 *
 * Always returns 0x00403000. Soft compile-time graphics API tag; does
 * not call the Vulkan loader or libc. No parent wires.
 */
uint32_t
gj_vk_api_1_3_u(void)
{
	(void)NULL;
	return b5364_vk_api_1_3();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_api_1_3_u(void)
    __attribute__((alias("gj_vk_api_1_3_u")));
