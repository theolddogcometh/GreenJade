/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8917: Vulkan R8G8B8A8 format soft id (no ICD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_format_rgba8_id_8917(void);
 *     - Returns VK_FORMAT_R8G8B8A8_UNORM (37 / 0x25). Soft format id
 *       stub for freestanding RGBA8 path tagging.
 *   uint32_t __gj_vk_format_rgba8_id_8917  (alias)
 *   __libcgj_batch8917_marker = "libcgj-batch8917"
 *
 * Exclusive continuum CREATE-ONLY (8911-8920: vulkan soft id stubs, no
 * ICD). Unique gj_vk_format_rgba8_id_8917 surface only; no multi-def.
 * Distinct from gj_vk_format_is_color_hint (batch1962). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not load a Vulkan ICD.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8917_marker[] = "libcgj-batch8917";

/* Vulkan VK_FORMAT_R8G8B8A8_UNORM. */
#define B8917_FORMAT_RGBA8  ((uint32_t)37u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8917_format_rgba8(void)
{
	return B8917_FORMAT_RGBA8;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_format_rgba8_id_8917 - report VK_FORMAT_R8G8B8A8_UNORM soft id.
 *
 * Always returns 37 (0x25). Soft compile-time format id stub; does not
 * call the Vulkan loader, ICD, or libc. No parent wires.
 */
uint32_t
gj_vk_format_rgba8_id_8917(void)
{
	(void)NULL;
	return b8917_format_rgba8();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vk_format_rgba8_id_8917(void)
    __attribute__((alias("gj_vk_format_rgba8_id_8917")));
