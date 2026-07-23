/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5859: Deck core Vulkan extension pack
 * (hash table match + count stubs).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_vk_ext_pack_count_5859(void);
 *     - Returns 7 (number of core Deck extension digests in this pack:
 *       swapchain, dynamic_rendering, timeline_semaphore,
 *       descriptor_indexing, synchronization2, hdr_metadata,
 *       present_wait). Soft constant only.
 *   uint32_t gj_deck_vk_ext_pack_has_5859(uint32_t hash);
 *     - Return 1 if hash matches any core Deck extension digest in the
 *       pack, else 0.
 *   uint32_t gj_deck_vk_ext_pack_bit_5859(uint32_t hash);
 *     - Return a single bit 0..6 for a known pack digest (bit index
 *       matches wave order 5852..5858), or 0xffffffff if unknown.
 *   uint32_t __gj_deck_vk_ext_pack_count_5859  (alias)
 *   uint32_t __gj_deck_vk_ext_pack_has_5859  (alias)
 *   uint32_t __gj_deck_vk_ext_pack_bit_5859  (alias)
 *   __libcgj_batch5859_marker = "libcgj-batch5859"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5859 surfaces only; no multi-def.
 * Does not redefine hash/match symbols from 5852-5858. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5859_marker[] = "libcgj-batch5859";

#define B5859_PACK_COUNT  7u

/* Core Deck pack digests (FNV-1a 32 of canonical extension names). */
#define B5859_H_SWAPCHAIN      0x2fab4bbdu /* VK_KHR_swapchain */
#define B5859_H_DYN_RENDER     0x5a58b403u /* VK_KHR_dynamic_rendering */
#define B5859_H_TIMELINE_SEM   0x20a7a39du /* VK_KHR_timeline_semaphore */
#define B5859_H_DESC_IDX       0x3159762bu /* VK_EXT_descriptor_indexing */
#define B5859_H_SYNC2          0x4e0b3089u /* VK_KHR_synchronization2 */
#define B5859_H_HDR_META       0xdefe6055u /* VK_EXT_hdr_metadata */
#define B5859_H_PRESENT_WAIT   0xed74579eu /* VK_KHR_present_wait */

#define B5859_UNKNOWN_BIT      0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5859_count(void)
{
	return B5859_PACK_COUNT;
}

static uint32_t
b5859_bit(uint32_t u32Hash)
{
	if (u32Hash == B5859_H_SWAPCHAIN) {
		return 0u;
	}
	if (u32Hash == B5859_H_DYN_RENDER) {
		return 1u;
	}
	if (u32Hash == B5859_H_TIMELINE_SEM) {
		return 2u;
	}
	if (u32Hash == B5859_H_DESC_IDX) {
		return 3u;
	}
	if (u32Hash == B5859_H_SYNC2) {
		return 4u;
	}
	if (u32Hash == B5859_H_HDR_META) {
		return 5u;
	}
	if (u32Hash == B5859_H_PRESENT_WAIT) {
		return 6u;
	}
	return B5859_UNKNOWN_BIT;
}

static uint32_t
b5859_has(uint32_t u32Hash)
{
	return (b5859_bit(u32Hash) != B5859_UNKNOWN_BIT) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_vk_ext_pack_count_5859 - number of core Deck VK ext digests.
 *
 * Always returns 7. Soft product pack size; not a live
 * vkEnumerateDeviceExtensionProperties count. No parent wires.
 */
uint32_t
gj_deck_vk_ext_pack_count_5859(void)
{
	(void)NULL;
	return b5859_count();
}

/*
 * gj_deck_vk_ext_pack_has_5859 - is hash in the core Deck ext pack?
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 if hash is one of the seven core digests, else 0.
 */
uint32_t
gj_deck_vk_ext_pack_has_5859(uint32_t hash)
{
	return b5859_has(hash);
}

/*
 * gj_deck_vk_ext_pack_bit_5859 - pack bit index for a known digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 0..6 for known pack entries, or 0xffffffff if unknown.
 */
uint32_t
gj_deck_vk_ext_pack_bit_5859(uint32_t hash)
{
	return b5859_bit(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_vk_ext_pack_count_5859(void)
    __attribute__((alias("gj_deck_vk_ext_pack_count_5859")));

uint32_t __gj_deck_vk_ext_pack_has_5859(uint32_t hash)
    __attribute__((alias("gj_deck_vk_ext_pack_has_5859")));

uint32_t __gj_deck_vk_ext_pack_bit_5859(uint32_t hash)
    __attribute__((alias("gj_deck_vk_ext_pack_bit_5859")));
