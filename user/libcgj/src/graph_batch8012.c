/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8012: malloc alignment power-of-two check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_malloc_align_is_pow2_8012(size_t align);
 *     - Return 1 if align is a non-zero power of two; else 0.
 *   uint32_t gj_malloc_align_is_zero_8012(size_t align);
 *     - Return 1 if align == 0; else 0.
 *   uint32_t __gj_malloc_align_is_pow2_8012  (alias)
 *   uint32_t __gj_malloc_align_is_zero_8012  (alias)
 *   __libcgj_batch8012_marker = "libcgj-batch8012"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_malloc_align_*_8012 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8012_marker[] = "libcgj-batch8012";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8012_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	/* Classic power-of-two test: exactly one bit set. */
	if ((uAlign & (uAlign - 1u)) == 0u) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b8012_is_zero(size_t uAlign)
{
	return (uAlign == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_is_pow2_8012 - non-zero power-of-two alignment test.
 *
 * align: candidate alignment in bytes
 *
 * Returns 1 if align is a power of two and non-zero; else 0.
 * Soft pure-data predicate; does not call malloc. No parent wires.
 */
uint32_t
gj_malloc_align_is_pow2_8012(size_t align)
{
	(void)NULL;
	return b8012_is_pow2(align);
}

/*
 * gj_malloc_align_is_zero_8012 - zero-alignment probe.
 *
 * Returns 1 if align == 0 (invalid for memalign/posix_memalign); else 0.
 */
uint32_t
gj_malloc_align_is_zero_8012(size_t align)
{
	return b8012_is_zero(align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_malloc_align_is_pow2_8012(size_t align)
    __attribute__((alias("gj_malloc_align_is_pow2_8012")));

uint32_t __gj_malloc_align_is_zero_8012(size_t align)
    __attribute__((alias("gj_malloc_align_is_zero_8012")));
