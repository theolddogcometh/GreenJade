/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8016: posix_memalign alignment validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_posix_memalign_align_ok_8016(size_t align);
 *     - Return 1 if align is a power of two and a multiple of the soft
 *       sizeof(void *) (8); else 0. Mirrors POSIX posix_memalign rules.
 *   uint32_t gj_posix_memalign_einval_soft_8016(size_t align);
 *     - Soft EINVAL (22) when align is invalid; 0 when ok.
 *   uint32_t __gj_posix_memalign_align_ok_8016  (alias)
 *   uint32_t __gj_posix_memalign_einval_soft_8016  (alias)
 *   __libcgj_batch8016_marker = "libcgj-batch8016"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_posix_memalign_*_8016 surfaces only; no multi-def with
 * posix_memalign. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8016_marker[] = "libcgj-batch8016";

/* Soft LP64 sizeof(void *); soft POSIX EINVAL value. */
#define B8016_VOIDPTR  8u
#define B8016_EINVAL   22u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8016_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static uint32_t
b8016_ok(size_t uAlign)
{
	/*
	 * POSIX: alignment shall be a multiple of sizeof(void *) and a
	 * power of two. Soft voidptr = 8 (LP64); power-of-two multiple of
	 * 8 is equivalent to power-of-two with align >= 8.
	 */
	if (b8016_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	if ((uAlign % (size_t)B8016_VOIDPTR) != 0u) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b8016_einval(size_t uAlign)
{
	return (b8016_ok(uAlign) != 0u) ? 0u : B8016_EINVAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_posix_memalign_align_ok_8016 - soft posix_memalign alignment check.
 *
 * align: candidate alignment in bytes
 *
 * Returns 1 if valid under soft POSIX rules; else 0.
 * Soft pure-data predicate; does not call posix_memalign.
 * No parent wires.
 */
uint32_t
gj_posix_memalign_align_ok_8016(size_t align)
{
	(void)NULL;
	return b8016_ok(align);
}

/*
 * gj_posix_memalign_einval_soft_8016 - soft EINVAL for bad alignment.
 *
 * Returns 0 when align is ok; soft 22 (EINVAL) when invalid.
 */
uint32_t
gj_posix_memalign_einval_soft_8016(size_t align)
{
	return b8016_einval(align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_posix_memalign_align_ok_8016(size_t align)
    __attribute__((alias("gj_posix_memalign_align_ok_8016")));

uint32_t __gj_posix_memalign_einval_soft_8016(size_t align)
    __attribute__((alias("gj_posix_memalign_einval_soft_8016")));
