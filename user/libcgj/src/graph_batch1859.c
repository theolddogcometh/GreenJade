/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1859: index type size (2 or 4 bytes).
 *
 * Surface (unique symbols):
 *   uint32_t gj_index_type_size(uint32_t index_type);
 *     — Bytes per index element from a type code:
 *         0 → 2  (uint16 / VK_INDEX_TYPE_UINT16)
 *         1 → 4  (uint32 / VK_INDEX_TYPE_UINT32)
 *       Any other value returns 0 (unsupported / UINT8 not implied).
 *   uint32_t __gj_index_type_size  (alias)
 *   __libcgj_batch1859_marker = "libcgj-batch1859"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1859_marker[] = "libcgj-batch1859";

/* Local type codes (not VkIndexType values — freestanding, no vulkan.h). */
#define B1859_INDEX_U16  0u
#define B1859_INDEX_U32  1u

/* ---- freestanding helpers ---------------------------------------------- */

/* Map local index-type code to element size in bytes. */
static uint32_t
b1859_index_size(uint32_t u32Type)
{
	if (u32Type == B1859_INDEX_U16) {
		return 2u;
	}
	if (u32Type == B1859_INDEX_U32) {
		return 4u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_index_type_size — bytes per index (2 or 4).
 *
 * index_type: 0 = uint16 indices, 1 = uint32 indices
 * Returns 2, 4, or 0 if the type code is unrecognized.
 */
uint32_t
gj_index_type_size(uint32_t u32IndexType)
{
	(void)sizeof(NULL);
	return b1859_index_size(u32IndexType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_index_type_size(uint32_t u32IndexType)
    __attribute__((alias("gj_index_type_size")));
