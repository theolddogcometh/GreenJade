/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5412: Vulkan ICD enumerate / Deck default.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_icd_enum_count_5412(void);
 *     - Returns soft ICD enumerate count hint (always 1 for Deck path).
 *   uint32_t gj_vk_icd_layer_path_ok_5412(uint32_t path_len);
 *     - Returns 1 if explicit layer path length is non-zero and bounded.
 *   uint32_t gj_vk_icd_deck_default_5412(void);
 *     - Returns 1 (Deck default ICD selection product tag).
 *   uint32_t __gj_vk_icd_enum_count_5412  (alias)
 *   uint32_t __gj_vk_icd_layer_path_ok_5412  (alias)
 *   uint32_t __gj_vk_icd_deck_default_5412  (alias)
 *   __libcgj_batch5412_marker = "libcgj-batch5412"
 *
 * Vulkan ICD / instance / device / queue / swapchain Deck exclusive
 * CREATE-ONLY wave (5411-5420). Unique batch-suffixed symbols only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5412_marker[] = "libcgj-batch5412";

#define B5412_ICD_COUNT   1u
#define B5412_PATH_MAX    4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5412_enum_count(void)
{
	return B5412_ICD_COUNT;
}

static uint32_t
b5412_path_ok(uint32_t uLen)
{
	return (uLen > 0u && uLen <= B5412_PATH_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_icd_enum_count_5412 - soft ICD enumerate count for Deck path.
 *
 * Always returns 1 (single default ICD product stub).
 */
uint32_t
gj_vk_icd_enum_count_5412(void)
{
	(void)NULL;
	return b5412_enum_count();
}

/*
 * gj_vk_icd_layer_path_ok_5412 - 1 if explicit layer path length is ok.
 *
 * path_len: byte length of VK_LAYER_PATH-style directory string.
 */
uint32_t
gj_vk_icd_layer_path_ok_5412(uint32_t uPathLen)
{
	return b5412_path_ok(uPathLen);
}

/*
 * gj_vk_icd_deck_default_5412 - Deck default ICD selection product tag.
 *
 * Always returns 1. Soft compile-time presence only.
 */
uint32_t
gj_vk_icd_deck_default_5412(void)
{
	return 1u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_icd_enum_count_5412(void)
    __attribute__((alias("gj_vk_icd_enum_count_5412")));

uint32_t __gj_vk_icd_layer_path_ok_5412(uint32_t uPathLen)
    __attribute__((alias("gj_vk_icd_layer_path_ok_5412")));

uint32_t __gj_vk_icd_deck_default_5412(void)
    __attribute__((alias("gj_vk_icd_deck_default_5412")));
