/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1962: Vulkan format color soft hint.
 *
 * Surface (unique symbols):
 *   int gj_vk_format_is_color_hint(uint32_t fmt);
 *     — Return 1 if fmt is a non-zero VkFormat that is not in the core
 *       depth/stencil range (124..130), else 0. Soft color-vs-depth
 *       partition hint; does not validate sparse or extension formats.
 *   int __gj_vk_format_is_color_hint  (alias)
 *   __libcgj_batch1962_marker = "libcgj-batch1962"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Distinct from
 * gj_vk_format_is_depth_hint — unique name. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1962_marker[] = "libcgj-batch1962";

/* Core depth/stencil VkFormat band (see batch1961). */
#define B1962_DEPTH_LO  124u
#define B1962_DEPTH_HI  130u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if fmt is non-zero and outside the core depth/stencil band.
 * VK_FORMAT_UNDEFINED (0) is neither color nor depth for this hint.
 */
static int
b1962_is_color(uint32_t uFmt)
{
	if (uFmt == 0u) {
		return 0;
	}
	if (uFmt >= B1962_DEPTH_LO && uFmt <= B1962_DEPTH_HI) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_format_is_color_hint — 1 if fmt looks like a color VkFormat.
 *
 * fmt: VkFormat as uint32_t
 * Returns 1 when non-zero and not depth/stencil core, else 0.
 */
int
gj_vk_format_is_color_hint(uint32_t uFmt)
{
	(void)NULL;
	return b1962_is_color(uFmt);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vk_format_is_color_hint(uint32_t uFmt)
    __attribute__((alias("gj_vk_format_is_color_hint")));
