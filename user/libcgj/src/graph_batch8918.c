/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8918: Vulkan VK_SUCCESS soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_success_id_8918(void);
 *     - Returns the Vulkan success result code (0 / VK_SUCCESS).
 *   uint32_t __gj_vk_success_id_8918  (alias)
 *   __libcgj_batch8918_marker = "libcgj-batch8918"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_success_id_8918 surface only; no multi-def.
 * Distinct from gj_vk_success_u (batch5365). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8918_marker[] = "libcgj-batch8918";

/* Vulkan VK_SUCCESS result code. */
#define B8918_VK_SUCCESS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8918_vk_success(void)
{
	return B8918_VK_SUCCESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_success_id_8918 - report the Vulkan VK_SUCCESS result soft id.
 *
 * Always returns 0. Soft compile-time result id stub; does not call the
 * Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_success_id_8918(void)
{
	(void)NULL;
	return b8918_vk_success();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_success_id_8918(void)
    __attribute__((alias("gj_vk_success_id_8918")));
