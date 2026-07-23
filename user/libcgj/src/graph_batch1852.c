/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1852: GPU buffer total bytes from stride * count.
 *
 * Surface (unique symbols):
 *   uint64_t gj_buffer_stride_bytes(uint64_t stride, uint64_t count);
 *     — Return stride * count with overflow → 0. Zero stride or zero
 *       count yields 0 (empty range). Used for vertex/index/uniform
 *       buffer size hints from per-element stride and element count.
 *   uint64_t __gj_buffer_stride_bytes  (alias)
 *   __libcgj_batch1852_marker = "libcgj-batch1852"
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

const char __libcgj_batch1852_marker[] = "libcgj-batch1852";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked multiply; 0 on zero inputs or overflow. */
static uint64_t
b1852_stride_bytes(uint64_t u64Stride, uint64_t u64Count)
{
	if (u64Stride == 0u || u64Count == 0u) {
		return 0u;
	}
	if (u64Stride > (UINT64_MAX / u64Count)) {
		return 0u;
	}
	return u64Stride * u64Count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buffer_stride_bytes — total buffer bytes = stride * element count.
 *
 * stride: bytes between consecutive elements (vertex/index/uniform)
 * count:  number of elements
 * Returns product, or 0 if either is zero or the product would overflow.
 */
uint64_t
gj_buffer_stride_bytes(uint64_t u64Stride, uint64_t u64Count)
{
	(void)sizeof(NULL);
	return b1852_stride_bytes(u64Stride, u64Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_buffer_stride_bytes(uint64_t u64Stride, uint64_t u64Count)
    __attribute__((alias("gj_buffer_stride_bytes")));
