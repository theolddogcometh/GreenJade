/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5856: VK_KHR_synchronization2 extension
 * hash stub (Steam Deck barrier/sync2 path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_sync2_5856(void);
 *     - Returns the FNV-1a 32 digest of "VK_KHR_synchronization2"
 *       (0x4e0b3089). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_sync2_5856(uint32_t hash);
 *     - Return 1 if hash equals the synchronization2 digest, else 0.
 *   uint32_t __gj_vk_ext_hash_sync2_5856  (alias)
 *   uint32_t __gj_vk_ext_match_sync2_5856  (alias)
 *   __libcgj_batch5856_marker = "libcgj-batch5856"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5856 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5856_marker[] = "libcgj-batch5856";

/* FNV-1a 32 of "VK_KHR_synchronization2". */
#define B5856_HASH_SYNC2  0x4e0b3089u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5856_hash(void)
{
	return B5856_HASH_SYNC2;
}

static uint32_t
b5856_match(uint32_t u32Hash)
{
	return (u32Hash == B5856_HASH_SYNC2) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_sync2_5856 - soft VK_KHR_synchronization2 hash.
 *
 * Always returns 0x4e0b3089. Does not record pipeline barriers.
 * No parent wires.
 */
uint32_t
gj_vk_ext_hash_sync2_5856(void)
{
	(void)NULL;
	return b5856_hash();
}

/*
 * gj_vk_ext_match_sync2_5856 - test hash against sync2 digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_sync2_5856(uint32_t hash)
{
	return b5856_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_sync2_5856(void)
    __attribute__((alias("gj_vk_ext_hash_sync2_5856")));

uint32_t __gj_vk_ext_match_sync2_5856(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_sync2_5856")));
