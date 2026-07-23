/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1858: vertex input stride validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_vertex_stride_ok(uint32_t stride, uint32_t min_align,
 *                           uint32_t max_stride);
 *     — Return 1 if stride is a valid vertex binding stride:
 *         stride != 0, stride <= max_stride, and
 *         (min_align == 0 || stride % min_align == 0).
 *       Otherwise 0. Integer-only GPU input-state hint.
 *   int __gj_vertex_stride_ok  (alias)
 *   __libcgj_batch1858_marker = "libcgj-batch1858"
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

const char __libcgj_batch1858_marker[] = "libcgj-batch1858";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if stride nonzero, in range, and aligned to min_align (if any). */
static int
b1858_stride_ok(uint32_t u32Stride, uint32_t u32MinAlign, uint32_t u32Max)
{
	if (u32Stride == 0u) {
		return 0;
	}
	if (u32Stride > u32Max) {
		return 0;
	}
	if (u32MinAlign != 0u && (u32Stride % u32MinAlign) != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vertex_stride_ok — 1 if vertex binding stride is acceptable.
 *
 * stride:    VkVertexInputBindingDescription::stride candidate
 * min_align: required alignment of stride (0 = no alignment check)
 * max_stride: device maxVertexInputBindingStride (or similar limit)
 * Returns 1 if valid, else 0.
 */
int
gj_vertex_stride_ok(uint32_t u32Stride, uint32_t u32MinAlign, uint32_t u32Max)
{
	(void)sizeof(NULL);
	return b1858_stride_ok(u32Stride, u32MinAlign, u32Max);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vertex_stride_ok(uint32_t u32Stride, uint32_t u32MinAlign,
    uint32_t u32Max)
    __attribute__((alias("gj_vertex_stride_ok")));
