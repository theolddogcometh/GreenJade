/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5411: Vulkan ICD load probe for Deck path.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_icd_load_probe_5411(void);
 *     - Returns 1 (ICD loader path present as a soft product tag).
 *   uint32_t gj_vk_icd_lib_name_ok_5411(uint32_t name_len);
 *     - Returns 1 if name_len is in a plausible ICD .so name range.
 *   uint32_t gj_vk_icd_api_version_min_5411(void);
 *     - Returns packed min API version hint (Vulkan 1.1 style).
 *   uint32_t __gj_vk_icd_load_probe_5411  (alias)
 *   uint32_t __gj_vk_icd_lib_name_ok_5411  (alias)
 *   uint32_t __gj_vk_icd_api_version_min_5411  (alias)
 *   __libcgj_batch5411_marker = "libcgj-batch5411"
 *
 * Vulkan ICD / instance / device / queue / swapchain Deck exclusive
 * CREATE-ONLY wave (5411-5420). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / bare prior symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5411_marker[] = "libcgj-batch5411";

/* VK_MAKE_VERSION(1, 1, 0) style packed major/minor/patch. */
#define B5411_API_MIN  0x00401000u
#define B5411_NAME_MIN 4u
#define B5411_NAME_MAX 256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5411_probe(void)
{
	return 1u;
}

static uint32_t
b5411_name_ok(uint32_t uLen)
{
	return (uLen >= B5411_NAME_MIN && uLen <= B5411_NAME_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_icd_load_probe_5411 - soft ICD load path presence tag.
 *
 * Always returns 1. Not a real dlopen; product-path compile-time stub.
 */
uint32_t
gj_vk_icd_load_probe_5411(void)
{
	(void)NULL;
	return b5411_probe();
}

/*
 * gj_vk_icd_lib_name_ok_5411 - 1 if ICD library name length is plausible.
 *
 * name_len: byte length of the ICD shared-object name (no NUL counted).
 */
uint32_t
gj_vk_icd_lib_name_ok_5411(uint32_t uNameLen)
{
	return b5411_name_ok(uNameLen);
}

/*
 * gj_vk_icd_api_version_min_5411 - minimum ICD API version product hint.
 *
 * Returns packed Vulkan 1.1.0 style constant (0x00401000).
 */
uint32_t
gj_vk_icd_api_version_min_5411(void)
{
	return B5411_API_MIN;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_icd_load_probe_5411(void)
    __attribute__((alias("gj_vk_icd_load_probe_5411")));

uint32_t __gj_vk_icd_lib_name_ok_5411(uint32_t uNameLen)
    __attribute__((alias("gj_vk_icd_lib_name_ok_5411")));

uint32_t __gj_vk_icd_api_version_min_5411(void)
    __attribute__((alias("gj_vk_icd_api_version_min_5411")));
