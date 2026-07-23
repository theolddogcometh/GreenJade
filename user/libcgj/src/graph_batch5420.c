/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5420: Vulkan present modes + Deck surface ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_present_mode_count_5420(void);
 *     - Returns soft present-mode count product hint (always 3).
 *   uint32_t gj_vk_present_fifo_ok_5420(void);
 *     - Returns 1 (FIFO present mode always available product tag).
 *   uint32_t gj_vk_swapchain_format_hint_5420(void);
 *     - Returns soft surface format fourcc-style hint (B8G8R8A8_SRGB).
 *   uint32_t gj_vk_deck_surface_ready_5420(void);
 *     - Returns 1 (Deck swapchain surface path ready product lamp).
 *   uint32_t gj_batch_id_5420(void);
 *     - Returns the compile-time graph batch number for this TU (5420).
 *   uint32_t __gj_vk_present_mode_count_5420  (alias)
 *   uint32_t __gj_vk_present_fifo_ok_5420  (alias)
 *   uint32_t __gj_vk_swapchain_format_hint_5420  (alias)
 *   uint32_t __gj_vk_deck_surface_ready_5420  (alias)
 *   uint32_t __gj_batch_id_5420  (alias)
 *   __libcgj_batch5420_marker = "libcgj-batch5420"
 *
 * MILESTONE wrap for Vulkan ICD / instance / device / queue / swapchain
 * Deck exclusive CREATE-ONLY wave (5411-5420). Does NOT redefine bare
 * gj_batch_id / gj_graph_milestone — batch-suffixed only. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5420_marker[] = "libcgj-batch5420";

/* Soft present modes: IMMEDIATE, MAILBOX, FIFO product set size. */
#define B5420_MODE_COUNT   3u
/* VK_FORMAT_B8G8R8A8_SRGB enum value as commonly used. */
#define B5420_FMT_BGRA_SRGB  50u
#define B5420_BATCH_ID       5420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5420_id(void)
{
	return B5420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_present_mode_count_5420 - soft present mode count for Deck.
 *
 * Always returns 3.
 */
uint32_t
gj_vk_present_mode_count_5420(void)
{
	(void)NULL;
	return B5420_MODE_COUNT;
}

/*
 * gj_vk_present_fifo_ok_5420 - FIFO present mode available product tag.
 *
 * Always returns 1 (Vulkan requires FIFO support).
 */
uint32_t
gj_vk_present_fifo_ok_5420(void)
{
	return 1u;
}

/*
 * gj_vk_swapchain_format_hint_5420 - soft preferred surface format.
 *
 * Returns 50 (B8G8R8A8_SRGB style product constant).
 */
uint32_t
gj_vk_swapchain_format_hint_5420(void)
{
	return B5420_FMT_BGRA_SRGB;
}

/*
 * gj_vk_deck_surface_ready_5420 - Deck swapchain surface ready lamp.
 *
 * Always returns 1. Soft compile-time product status tag.
 */
uint32_t
gj_vk_deck_surface_ready_5420(void)
{
	return 1u;
}

/*
 * gj_batch_id_5420 - report this TU's graph batch number.
 *
 * Always returns 5420. Distinct from bare gj_batch_id / other _N ids.
 */
uint32_t
gj_batch_id_5420(void)
{
	return b5420_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_present_mode_count_5420(void)
    __attribute__((alias("gj_vk_present_mode_count_5420")));

uint32_t __gj_vk_present_fifo_ok_5420(void)
    __attribute__((alias("gj_vk_present_fifo_ok_5420")));

uint32_t __gj_vk_swapchain_format_hint_5420(void)
    __attribute__((alias("gj_vk_swapchain_format_hint_5420")));

uint32_t __gj_vk_deck_surface_ready_5420(void)
    __attribute__((alias("gj_vk_deck_surface_ready_5420")));

uint32_t __gj_batch_id_5420(void)
    __attribute__((alias("gj_batch_id_5420")));
