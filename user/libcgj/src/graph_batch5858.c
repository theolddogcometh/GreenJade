/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5858: VK_KHR_present_wait extension hash
 * stub (Steam Deck present-timing / FPS-cap path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_present_wait_5858(void);
 *     - Returns the FNV-1a 32 digest of "VK_KHR_present_wait"
 *       (0xed74579e). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_present_wait_5858(uint32_t hash);
 *     - Return 1 if hash equals the present_wait digest, else 0.
 *   uint32_t __gj_vk_ext_hash_present_wait_5858  (alias)
 *   uint32_t __gj_vk_ext_match_present_wait_5858  (alias)
 *   __libcgj_batch5858_marker = "libcgj-batch5858"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5858 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5858_marker[] = "libcgj-batch5858";

/* FNV-1a 32 of "VK_KHR_present_wait". */
#define B5858_HASH_PRESENT_WAIT  0xed74579eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5858_hash(void)
{
	return B5858_HASH_PRESENT_WAIT;
}

static uint32_t
b5858_match(uint32_t u32Hash)
{
	return (u32Hash == B5858_HASH_PRESENT_WAIT) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_present_wait_5858 - soft VK_KHR_present_wait hash.
 *
 * Always returns 0xed74579e. Does not wait on a present id.
 * No parent wires.
 */
uint32_t
gj_vk_ext_hash_present_wait_5858(void)
{
	(void)NULL;
	return b5858_hash();
}

/*
 * gj_vk_ext_match_present_wait_5858 - test hash against present_wait.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_present_wait_5858(uint32_t hash)
{
	return b5858_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_present_wait_5858(void)
    __attribute__((alias("gj_vk_ext_hash_present_wait_5858")));

uint32_t __gj_vk_ext_match_present_wait_5858(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_present_wait_5858")));
