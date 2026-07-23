/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5851: Vulkan extension name FNV-1a 32 hasher
 * (Steam Deck extension string wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vk_ext_name_hash_5851(const char *name);
 *     - FNV-1a 32-bit hash of a NUL-terminated Vulkan extension name
 *       string. Returns 0 on NULL name. Empty string yields the FNV-1a
 *       offset basis (0x811c9dc5). Soft pure-data only; does not call
 *       vkEnumerate* or touch a real VkInstance.
 *   uint32_t gj_batch_id_5851(void);
 *     - Returns the compile-time graph batch number (5851).
 *   uint32_t __gj_vk_ext_name_hash_5851  (alias)
 *   uint32_t __gj_batch_id_5851  (alias)
 *   __libcgj_batch5851_marker = "libcgj-batch5851"
 *
 * Exclusive continuum CREATE-ONLY (5851-5860: Vulkan extension string
 * hashes/stubs for Deck). Unique *_5851 surfaces only; no multi-def.
 * Local FNV-1a 32 only — does not redefine fnv1a_32 / gj_fnv1a64.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5851_marker[] = "libcgj-batch5851";

#define B5851_BATCH_ID      5851u
#define B5851_FNV32_OFFSET  0x811c9dc5u
#define B5851_FNV32_PRIME   0x01000193u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b5851_fnv1a32 — FNV-1a 32 over a NUL-terminated C string.
 * name must be non-NULL.
 */
static uint32_t
b5851_fnv1a32(const char *name)
{
	const unsigned char *p = (const unsigned char *)name;
	uint32_t h = B5851_FNV32_OFFSET;

	while (*p != 0u) {
		h ^= (uint32_t)*p;
		h *= B5851_FNV32_PRIME;
		p++;
	}
	return h;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_ext_name_hash_5851 - FNV-1a 32 of a Vulkan extension name.
 *
 * name: NUL-terminated extension string (e.g. "VK_KHR_swapchain")
 *
 * Returns the 32-bit digest, or 0 when name is NULL. Soft pure-data
 * hash only; not a live Vulkan extension query. No parent wires.
 */
uint32_t
gj_vk_ext_name_hash_5851(const char *name)
{
	(void)NULL;
	if (name == NULL) {
		return 0u;
	}
	return b5851_fnv1a32(name);
}

/*
 * gj_batch_id_5851 - report this TU's graph batch number.
 *
 * Always returns 5851.
 */
uint32_t
gj_batch_id_5851(void)
{
	return B5851_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_vk_ext_name_hash_5851(const char *name)
    __attribute__((alias("gj_vk_ext_name_hash_5851")));

uint32_t __gj_batch_id_5851(void)
    __attribute__((alias("gj_batch_id_5851")));
