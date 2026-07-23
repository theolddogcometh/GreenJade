/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5854: VK_KHR_timeline_semaphore extension
 * hash stub (Steam Deck GPU/CPU sync path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_hash_timeline_sem_5854(void);
 *     - Returns the FNV-1a 32 digest of "VK_KHR_timeline_semaphore"
 *       (0x20a7a39d). Soft compile-time constant.
 *   uint32_t gj_vk_ext_match_timeline_sem_5854(uint32_t hash);
 *     - Return 1 if hash equals the timeline_semaphore digest, else 0.
 *   uint32_t __gj_vk_ext_hash_timeline_sem_5854  (alias)
 *   uint32_t __gj_vk_ext_match_timeline_sem_5854  (alias)
 *   __libcgj_batch5854_marker = "libcgj-batch5854"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5854 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5854_marker[] = "libcgj-batch5854";

/* FNV-1a 32 of "VK_KHR_timeline_semaphore". */
#define B5854_HASH_TIMELINE_SEM  0x20a7a39du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5854_hash(void)
{
	return B5854_HASH_TIMELINE_SEM;
}

static uint32_t
b5854_match(uint32_t u32Hash)
{
	return (u32Hash == B5854_HASH_TIMELINE_SEM) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_hash_timeline_sem_5854 - soft VK_KHR_timeline_semaphore hash.
 *
 * Always returns 0x20a7a39d. Does not create a timeline semaphore.
 * No parent wires.
 */
uint32_t
gj_vk_ext_hash_timeline_sem_5854(void)
{
	(void)NULL;
	return b5854_hash();
}

/*
 * gj_vk_ext_match_timeline_sem_5854 - test hash against timeline digest.
 *
 * hash: candidate FNV-1a 32 of an extension name
 *
 * Returns 1 on match, else 0.
 */
uint32_t
gj_vk_ext_match_timeline_sem_5854(uint32_t hash)
{
	return b5854_match(hash);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_hash_timeline_sem_5854(void)
    __attribute__((alias("gj_vk_ext_hash_timeline_sem_5854")));

uint32_t __gj_vk_ext_match_timeline_sem_5854(uint32_t hash)
    __attribute__((alias("gj_vk_ext_match_timeline_sem_5854")));
