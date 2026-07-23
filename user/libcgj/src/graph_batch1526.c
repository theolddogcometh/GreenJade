/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1526: slab class index → object size.
 *
 * Surface (unique symbols):
 *   size_t gj_slab_size_for_index(size_t index);
 *     — Inverse of gj_slab_index_for_size (batch1525). Classes are
 *       power-of-two starting at 8 bytes:
 *         index 0 → 8, 1 → 16, 2 → 32, ... size = 8 << index
 *       Overflow (shift would exceed size_t) → 0.
 *   size_t __gj_slab_size_for_index  (alias)
 *   __libcgj_batch1526_marker = "libcgj-batch1526"
 *
 * Distinct from gj_slab_obj_size / gj_slab_init. Unique surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1526_marker[] = "libcgj-batch1526";

/* Minimum slab class size (bytes); must match batch1525. */
#define B1526_MIN_CLASS 8u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_size_for_index — slab class index → object size in bytes.
 *
 * uIndex: class index from gj_slab_index_for_size
 *
 * Returns 8 << uIndex, or 0 on shift overflow.
 */
size_t
gj_slab_size_for_index(size_t uIndex)
{
	size_t uShiftMax;

	/* bits in size_t minus the 3 bits already used by MIN_CLASS (=8) */
	uShiftMax = (sizeof(size_t) * 8u) - 1u;
	/* 8 << uIndex == 1 << (uIndex + 3); need uIndex + 3 <= max bit */
	if (uIndex > (uShiftMax - 3u)) {
		return 0u;
	}
	return (size_t)(B1526_MIN_CLASS << uIndex);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_size_for_index(size_t uIndex)
    __attribute__((alias("gj_slab_size_for_index")));
