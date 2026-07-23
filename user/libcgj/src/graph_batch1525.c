/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1525: map allocation size to slab class index.
 *
 * Surface (unique symbols):
 *   size_t gj_slab_index_for_size(size_t size);
 *     — Map a requested object size to a slab size-class index.
 *       Classes are power-of-two starting at 8 bytes:
 *         index 0 → 8, 1 → 16, 2 → 32, ... up to index 28 → 2^31
 *       (on 64-bit, index 29 → 2^32 and so on, capped by SIZE_MAX).
 *       size == 0 → index 0 (minimum class). Sizes that exceed the
 *       largest representable power-of-two class → (size_t)-1.
 *   size_t __gj_slab_index_for_size  (alias)
 *   __libcgj_batch1525_marker = "libcgj-batch1525"
 *
 * Distinct from gj_slab_init / gj_slab_alloc / gj_slab_obj_size.
 * Pair with gj_slab_size_for_index (batch1526). Unique surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1525_marker[] = "libcgj-batch1525";

/* Minimum slab class size (bytes). */
#define B1525_MIN_CLASS 8u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round cbSize up to the next power of two (cbSize >= 1).
 * Returns 0 on overflow (no power of two >= cbSize in size_t).
 */
static size_t
b1525_next_pow2(size_t cbSize)
{
	size_t uV;

	if (cbSize <= 1u) {
		return 1u;
	}
	/* Already power of two? */
	if ((cbSize & (cbSize - 1u)) == 0u) {
		return cbSize;
	}
	/* Fill bits below MSB, then +1. */
	uV = cbSize - 1u;
	uV |= uV >> 1;
	uV |= uV >> 2;
	uV |= uV >> 4;
	uV |= uV >> 8;
	uV |= uV >> 16;
#if SIZE_MAX > 0xFFFFFFFFu
	uV |= uV >> 32;
#endif
	if (uV == SIZE_MAX) {
		return 0u; /* overflow: no next power of two */
	}
	return uV + 1u;
}

/*
 * log2 of a power-of-two value (uPow2 must be power of two and > 0).
 */
static size_t
b1525_log2_pow2(size_t uPow2)
{
	size_t uN;

	uN = 0u;
	while (uPow2 > 1u) {
		uPow2 >>= 1;
		uN++;
	}
	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_index_for_size — size → slab class index.
 *
 * cbSize: requested object size in bytes
 *
 * Classes: index i holds objects of size max(8, next_pow2(size)).
 * Index 0 is the 8-byte class (log2(8) - 3 = 0).
 * Returns (size_t)-1 if no class can hold cbSize.
 */
size_t
gj_slab_index_for_size(size_t cbSize)
{
	size_t uClass;
	size_t uLog;

	if (cbSize <= B1525_MIN_CLASS) {
		return 0u;
	}

	uClass = b1525_next_pow2(cbSize);
	if (uClass == 0u || uClass < B1525_MIN_CLASS) {
		return (size_t)-1;
	}

	/* index = log2(class) - log2(MIN_CLASS) = log2(class) - 3 */
	uLog = b1525_log2_pow2(uClass);
	if (uLog < 3u) {
		return 0u;
	}
	return uLog - 3u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_index_for_size(size_t cbSize)
    __attribute__((alias("gj_slab_index_for_size")));
