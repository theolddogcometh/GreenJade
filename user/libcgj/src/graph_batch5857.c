/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5857: VK_EXT_hdr_metadata extension hash
 * stub (Steam Deck OLED HDR present path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_hdr_meta_5857(void);
 *     - Returns the FNV-1a 32 digest of "VK_EXT_hdr_metadata"
 *       (0xdefe6055). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_hdr_meta_5857(uint32_t hash);
 *     - Return 1 if hash equals the hdr_metadata digest, else 0.
 *   uint32_t __gj_vk_ext_hash_hdr_meta_5857  (alias)
 *   uint32_t __gj_vk_ext_match_hdr_meta_5857  (alias)
 *   __libcgj_batch5857_marker = "libcgj-batch5857"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5857 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5857_marker[] = "libcgj-batch5857";

/* FNV-1a 32 of "VK_EXT_hdr_metadata". */
#define B5857_HASH_HDR_META  0xdefe6055u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5857_hash(void)
{
	return B5857_HASH_HDR_META;
}

static uint32_t
b5857_match(uint32_t u32Hash)
{
	return (u32Hash == B5857_HASH_HDR_META) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_hdr_meta_5857 - soft VK_EXT_hdr_metadata hash.
 *
 * Always returns 0xdefe6055. Does not set HDR metadata on a swapchain.
 * Deck OLED soft product tag only. No parent wires.
 */
uint32_t
gj_vk_ext_hash_hdr_meta_5857(void)
{
	(void)NULL;
	return b5857_hash();
}

/*
 * gj_vk_ext_match_hdr_meta_5857 - test hash against hdr_meta digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_hdr_meta_5857(uint32_t hash)
{
	return b5857_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_hdr_meta_5857(void)
    __attribute__((alias("gj_vk_ext_hash_hdr_meta_5857")));

uint32_t __gj_vk_ext_match_hdr_meta_5857(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_hdr_meta_5857")));
