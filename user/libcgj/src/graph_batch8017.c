/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8017: malloc chunk / memalign size stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_malloc_chunk_overhead_8017(void);
 *     - Soft chunk header overhead (16 bytes on LP64).
 *   size_t   gj_malloc_chunk_align_size_8017(size_t size);
 *     - Soft request size + overhead, rounded up to default align 16.
 *       Overflow → 0.
 *   size_t   gj_memalign_request_size_8017(size_t size, size_t align);
 *     - Soft min size for an aligned request: max(size, align) then
 *       align-up to align. Invalid align → 0. Overflow → 0.
 *   uint32_t / size_t __gj_*_8017  (aliases)
 *   __libcgj_batch8017_marker = "libcgj-batch8017"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique *_8017 surfaces only; no multi-def with malloc/memalign.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8017_marker[] = "libcgj-batch8017";

#define B8017_OVERHEAD  16u
#define B8017_DEFAULT   16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8017_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static size_t
b8017_align_up(size_t uN, size_t uAlign)
{
	size_t uMask;
	size_t uSum;

	if (b8017_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uMask = uAlign - 1u;
	uSum = uN + uMask;
	if (uSum < uN) {
		return 0u;
	}
	return uSum & ~uMask;
}

static uint32_t
b8017_overhead(void)
{
	return B8017_OVERHEAD;
}

static size_t
b8017_chunk_align(size_t uSize)
{
	size_t uNeed;

	uNeed = uSize + (size_t)B8017_OVERHEAD;
	if (uNeed < uSize) {
		return 0u;
	}
	return b8017_align_up(uNeed, (size_t)B8017_DEFAULT);
}

static size_t
b8017_memalign_req(size_t uSize, size_t uAlign)
{
	size_t uBase;

	if (b8017_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uBase = (uSize > uAlign) ? uSize : uAlign;
	return b8017_align_up(uBase, uAlign);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_chunk_overhead_8017 - soft malloc chunk header bytes.
 *
 * Always returns 16. Soft pure-data tag; does not inspect a live heap.
 * No parent wires.
 */
uint32_t
gj_malloc_chunk_overhead_8017(void)
{
	(void)NULL;
	return b8017_overhead();
}

/*
 * gj_malloc_chunk_align_size_8017 - soft size+overhead aligned to 16.
 *
 * size: user request size
 *
 * Returns aligned chunk size, or 0 on overflow.
 */
size_t
gj_malloc_chunk_align_size_8017(size_t size)
{
	return b8017_chunk_align(size);
}

/*
 * gj_memalign_request_size_8017 - soft minimum memalign request size.
 *
 * size:  payload size
 * align: power-of-two alignment
 *
 * Returns max(size, align) rounded up to align; invalid/overflow → 0.
 */
size_t
gj_memalign_request_size_8017(size_t size, size_t align)
{
	return b8017_memalign_req(size, align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_malloc_chunk_overhead_8017(void)
    __attribute__((alias("gj_malloc_chunk_overhead_8017")));

size_t __gj_malloc_chunk_align_size_8017(size_t size)
    __attribute__((alias("gj_malloc_chunk_align_size_8017")));

size_t __gj_memalign_request_size_8017(size_t size, size_t align)
    __attribute__((alias("gj_memalign_request_size_8017")));
