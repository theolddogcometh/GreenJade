/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5855: VK_EXT_descriptor_indexing extension
 * hash stub (Steam Deck bindless/descriptor path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_desc_idx_5855(void);
 *     - Returns the FNV-1a 32 digest of "VK_EXT_descriptor_indexing"
 *       (0x3159762b). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_desc_idx_5855(uint32_t hash);
 *     - Return 1 if hash equals the descriptor_indexing digest, else 0.
 *   uint32_t __gj_vk_ext_hash_desc_idx_5855  (alias)
 *   uint32_t __gj_vk_ext_match_desc_idx_5855  (alias)
 *   __libcgj_batch5855_marker = "libcgj-batch5855"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5855 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5855_marker[] = "libcgj-batch5855";

/* FNV-1a 32 of "VK_EXT_descriptor_indexing". */
#define B5855_HASH_DESC_IDX  0x3159762bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5855_hash(void)
{
	return B5855_HASH_DESC_IDX;
}

static uint32_t
b5855_match(uint32_t u32Hash)
{
	return (u32Hash == B5855_HASH_DESC_IDX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_desc_idx_5855 - soft VK_EXT_descriptor_indexing hash.
 *
 * Always returns 0x3159762b. Does not update descriptor sets.
 * No parent wires.
 */
uint32_t
gj_vk_ext_hash_desc_idx_5855(void)
{
	(void)NULL;
	return b5855_hash();
}

/*
 * gj_vk_ext_match_desc_idx_5855 - test hash against desc_idx digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_desc_idx_5855(uint32_t hash)
{
	return b5855_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_desc_idx_5855(void)
    __attribute__((alias("gj_vk_ext_hash_desc_idx_5855")));

uint32_t __gj_vk_ext_match_desc_idx_5855(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_desc_idx_5855")));
