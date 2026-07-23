/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5363: Vulkan API version 1.2 packed constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_api_1_2_u(void);
 *     - Returns the packed Vulkan API version 1.2 value (0x00402000),
 *       matching VK_API_VERSION_1_2 / VK_MAKE_API_VERSION(0,1,2,0).
 *   uint32_t __gj_vk_api_1_2_u  (alias)
 *   __libcgj_batch5363_marker = "libcgj-batch5363"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Unique gj_vk_api_1_2_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5363_marker[] = "libcgj-batch5363";

/* Vulkan VK_API_VERSION_1_2 = VK_MAKE_API_VERSION(0, 1, 2, 0). */
#define B5363_VK_API_1_2  ((uint32_t)0x00402000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5363_vk_api_1_2(void)
{
	return B5363_VK_API_1_2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_api_1_2_u - report packed Vulkan API version 1.2.
 *
 * Always returns 0x00402000. Soft compile-time graphics API tag; does
 * not call the Vulkan loader or libc. No parent wires.
 */
uint32_t
gj_vk_api_1_2_u(void)
{
	(void)NULL;
	return b5363_vk_api_1_2();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_api_1_2_u(void)
    __attribute__((alias("gj_vk_api_1_2_u")));
