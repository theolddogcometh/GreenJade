/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8015: malloc alignment pad bytes stub.
 *
 * Surface (unique symbols):
 *   size_t gj_malloc_align_pad_8015(size_t n, size_t align);
 *     - Bytes of padding needed to lift n to the next multiple of
 *       align (0 if already aligned). Invalid align → 0.
 *   size_t gj_malloc_align_padded_size_8015(size_t n, size_t align);
 *     - n + pad (same as align_up when valid); overflow → 0.
 *   size_t __gj_malloc_align_pad_8015  (alias)
 *   size_t __gj_malloc_align_padded_size_8015  (alias)
 *   __libcgj_batch8015_marker = "libcgj-batch8015"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_malloc_align_*_8015 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8015_marker[] = "libcgj-batch8015";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8015_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static size_t
b8015_pad(size_t uN, size_t uAlign)
{
	size_t uMask;
	size_t uRem;

	if (b8015_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uMask = uAlign - 1u;
	uRem = uN & uMask;
	if (uRem == 0u) {
		return 0u;
	}
	return uAlign - uRem;
}

static size_t
b8015_padded(size_t uN, size_t uAlign)
{
	size_t uPad;
	size_t uSum;

	if (b8015_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uPad = b8015_pad(uN, uAlign);
	uSum = uN + uPad;
	if (uSum < uN) {
		return 0u;
	}
	return uSum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_pad_8015 - soft pad bytes to next alignment boundary.
 *
 * n:     current size/offset
 * align: power-of-two alignment
 *
 * Returns pad in [0, align). Invalid align → 0.
 * Soft pure-data math; does not allocate. No parent wires.
 */
size_t
gj_malloc_align_pad_8015(size_t n, size_t align)
{
	(void)NULL;
	return b8015_pad(n, align);
}

/*
 * gj_malloc_align_padded_size_8015 - soft n rounded up with pad.
 *
 * Equivalent to n + pad when valid; overflow or bad align → 0.
 */
size_t
gj_malloc_align_padded_size_8015(size_t n, size_t align)
{
	return b8015_padded(n, align);
}

/* ---- underscored aliases ----------------------------------------------- */

size_t __gj_malloc_align_pad_8015(size_t n, size_t align)
    __attribute__((alias("gj_malloc_align_pad_8015")));

size_t __gj_malloc_align_padded_size_8015(size_t n, size_t align)
    __attribute__((alias("gj_malloc_align_padded_size_8015")));
