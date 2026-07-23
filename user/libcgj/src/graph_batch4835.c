/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4835: cache-line span of a byte range.
 *
 * Surface (unique symbols):
 *   size_t gj_cacheline_span_u(uintptr_t addr, size_t len);
 *     - Number of distinct cache lines touched by [addr, addr+len).
 *       len == 0 → 0. Address-space overflow treats the last byte as
 *       UINTPTR_MAX.
 *   size_t __gj_cacheline_span_u  (alias)
 *   __libcgj_batch4835_marker = "libcgj-batch4835"
 *
 * Exclusive continuum CREATE-ONLY (4831-4840: cacheline_size_u,
 * cacheline_align_u, cacheline_index_u, cacheline_offset_u,
 * cacheline_span_u, prefetch_hint_read_u, prefetch_hint_write_u,
 * false_sharing_risk_u, pad_to_line_u, batch_id_4840). Unique
 * gj_cacheline_span_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4835_marker[] = "libcgj-batch4835";

/* log2 of assumed cache-line width (64). */
#define B4835_CL_SHIFT  6u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4835_cacheline_span(uintptr_t uAddr, size_t uLen)
{
	uintptr_t uLast;
	uintptr_t uFirstIdx;
	uintptr_t uLastIdx;

	if (uLen == 0u) {
		return 0u;
	}

	/* Last byte of the half-open range [addr, addr+len). */
	if (uLen - 1u > (size_t)(UINTPTR_MAX - uAddr)) {
		uLast = UINTPTR_MAX;
	} else {
		uLast = uAddr + (uintptr_t)(uLen - 1u);
	}

	uFirstIdx = uAddr >> B4835_CL_SHIFT;
	uLastIdx = uLast >> B4835_CL_SHIFT;
	return (size_t)(uLastIdx - uFirstIdx + 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cacheline_span_u - count cache lines touched by [addr, addr+len).
 *
 * addr: start address (integer form).
 * len:  byte length of the range (0 → 0 lines).
 *
 * Returns the inclusive line-index distance plus one. No parent wires.
 */
size_t
gj_cacheline_span_u(uintptr_t uAddr, size_t uLen)
{
	(void)NULL;
	return b4835_cacheline_span(uAddr, uLen);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_cacheline_span_u(uintptr_t uAddr, size_t uLen)
    __attribute__((alias("gj_cacheline_span_u")));
