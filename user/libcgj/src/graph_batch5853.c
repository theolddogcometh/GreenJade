/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5853: VK_KHR_dynamic_rendering extension
 * hash stub (Steam Deck modern render-pass-less path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_dyn_render_5853(void);
 *     - Returns the FNV-1a 32 digest of "VK_KHR_dynamic_rendering"
 *       (0x5a58b403). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_dyn_render_5853(uint32_t hash);
 *     - Return 1 if hash equals the dynamic_rendering digest, else 0.
 *   uint32_t __gj_vk_ext_hash_dyn_render_5853  (alias)
 *   uint32_t __gj_vk_ext_match_dyn_render_5853  (alias)
 *   __libcgj_batch5853_marker = "libcgj-batch5853"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5853 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5853_marker[] = "libcgj-batch5853";

/* FNV-1a 32 of "VK_KHR_dynamic_rendering". */
#define B5853_HASH_DYN_RENDER  0x5a58b403u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5853_hash(void)
{
	return B5853_HASH_DYN_RENDER;
}

static uint32_t
b5853_match(uint32_t u32Hash)
{
	return (u32Hash == B5853_HASH_DYN_RENDER) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_dyn_render_5853 - soft VK_KHR_dynamic_rendering hash.
 *
 * Always returns 0x5a58b403. Does not begin a dynamic render pass.
 * No parent wires.
 */
uint32_t
gj_vk_ext_hash_dyn_render_5853(void)
{
	(void)NULL;
	return b5853_hash();
}

/*
 * gj_vk_ext_match_dyn_render_5853 - test hash against dyn_render digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_dyn_render_5853(uint32_t hash)
{
	return b5853_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_dyn_render_5853(void)
    __attribute__((alias("gj_vk_ext_hash_dyn_render_5853")));

uint32_t __gj_vk_ext_match_dyn_render_5853(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_dyn_render_5853")));
