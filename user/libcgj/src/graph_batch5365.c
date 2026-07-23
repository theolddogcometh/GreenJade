/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5365: Vulkan VK_SUCCESS result constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_success_u(void);
 *     - Returns the Vulkan success result code (0 / VK_SUCCESS).
 *   uint32_t __gj_vk_success_u  (alias)
 *   __libcgj_batch5365_marker = "libcgj-batch5365"
 *
 * Exclusive continuum CREATE-ONLY (5361-5370: vk_api_1_0_u, vk_api_1_1_u,
 * vk_api_1_2_u, vk_api_1_3_u, vk_success_u, vk_is_success_u, gl_true_u,
 * gl_false_u, gl_triangles_u, batch_id_5370). Unique gj_vk_success_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5365_marker[] = "libcgj-batch5365";

/* Vulkan VK_SUCCESS result code. */
#define B5365_VK_SUCCESS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5365_vk_success(void)
{
	return B5365_VK_SUCCESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_success_u - report the Vulkan VK_SUCCESS result code.
 *
 * Always returns 0. Soft compile-time graphics result tag; does not
 * call the Vulkan loader or libc. No parent wires.
 */
uint32_t
gj_vk_success_u(void)
{
	(void)NULL;
	return b5365_vk_success();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_success_u(void)
    __attribute__((alias("gj_vk_success_u")));
