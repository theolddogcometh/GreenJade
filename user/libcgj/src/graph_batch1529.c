/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1529: pool object pointer → slot index.
 *
 * Surface (unique symbols):
 *   size_t gj_pool_obj_index(const void *base, const void *obj,
 *                            size_t stride);
 *     — Given a dense pool region starting at base with fixed object
 *       stride, return the index of obj: (obj - base) / stride.
 *       Returns (size_t)-1 on NULL base/obj, stride == 0, obj < base,
 *       or when the offset is not an exact multiple of stride.
 *   size_t __gj_pool_obj_index  (alias)
 *   __libcgj_batch1529_marker = "libcgj-batch1529"
 *
 * Distinct from gj_pool_alloc / gj_pool_free (batch546 freelist).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1529_marker[] = "libcgj-batch1529";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pool_obj_index — pointer → pool slot index given base and stride.
 *
 * pBase:   pool region base address
 * pObj:    pointer to an object within the pool
 * cbStride: fixed object size / stride in bytes (must be > 0)
 *
 * Returns index, or (size_t)-1 on invalid args / misalignment / below base.
 */
size_t
gj_pool_obj_index(const void *pBase, const void *pObj, size_t cbStride)
{
	const unsigned char *pB;
	const unsigned char *pO;
	size_t uOff;

	if (pBase == NULL || pObj == NULL || cbStride == 0u) {
		return (size_t)-1;
	}

	pB = (const unsigned char *)pBase;
	pO = (const unsigned char *)pObj;

	/* obj must not be before base (unsigned address compare). */
	if ((uintptr_t)pO < (uintptr_t)pB) {
		return (size_t)-1;
	}

	uOff = (size_t)((uintptr_t)pO - (uintptr_t)pB);
	if ((uOff % cbStride) != 0u) {
		return (size_t)-1;
	}
	return uOff / cbStride;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_pool_obj_index(const void *pBase, const void *pObj,
			   size_t cbStride)
    __attribute__((alias("gj_pool_obj_index")));
