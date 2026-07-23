/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3977: slab element byte offset from index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slab_offset_u(size_t idx, size_t stride);
 *     - Return the byte offset of element idx in a fixed-stride slab:
 *       idx * stride. Overflow of the product → (size_t)-1. stride == 0
 *       → (size_t)-1. Inverse of gj_slab_idx_u (batch3343).
 *   size_t __gj_slab_offset_u  (alias)
 *   __libcgj_batch3977_marker = "libcgj-batch3977"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Distinct from
 * gj_slab_idx_u (batch3343 address→index) and gj_slab_obj_size
 * (batch974) — unique gj_slab_offset_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3977_marker[] = "libcgj-batch3977";

#define B3977_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3977_offset(size_t uIdx, size_t uStride)
{
	if (uStride == 0u) {
		return B3977_FAIL;
	}
	if (uIdx > (SIZE_MAX / uStride)) {
		return B3977_FAIL;
	}
	return uIdx * uStride;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_offset_u - byte offset of element idx at fixed stride.
 *
 * idx:    zero-based element index
 * stride: element size in bytes (must be > 0)
 *
 * Returns idx * stride, or (size_t)-1 on bad stride / overflow.
 * No parent wires.
 */
size_t
gj_slab_offset_u(size_t uIdx, size_t uStride)
{
	(void)NULL;
	return b3977_offset(uIdx, uStride);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_offset_u(size_t uIdx, size_t uStride)
    __attribute__((alias("gj_slab_offset_u")));
