/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8011: malloc default alignment constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_malloc_align_default_8011(void);
 *     - Soft default malloc alignment (16), typical glibc MALLOC_ALIGNMENT
 *       on 64-bit desktops (2 * sizeof(size_t)).
 *   uint32_t gj_malloc_align_min_8011(void);
 *     - Soft minimum alignment (8), sizeof(void *) on LP64.
 *   uint32_t gj_malloc_align_voidptr_8011(void);
 *     - Soft sizeof(void *) tag (8).
 *   uint32_t __gj_malloc_align_*_8011  (aliases)
 *   __libcgj_batch8011_marker = "libcgj-batch8011"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs —
 * default_const_8011, is_pow2_8012, align_up_8013, align_down_8014,
 * pad_8015, posix_ok_8016, chunk_8017, ptr_align_8018, clamp_8019,
 * continuum_8020). Unique gj_malloc_align_*_8011 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8011_marker[] = "libcgj-batch8011";

/* Soft LP64 malloc alignment tags (pure data; not queried from libc). */
#define B8011_ALIGN_DEFAULT  16u
#define B8011_ALIGN_MIN      8u
#define B8011_ALIGN_VOIDPTR  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8011_default(void)
{
	return B8011_ALIGN_DEFAULT;
}

static uint32_t
b8011_min(void)
{
	return B8011_ALIGN_MIN;
}

static uint32_t
b8011_voidptr(void)
{
	return B8011_ALIGN_VOIDPTR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_default_8011 - soft default malloc alignment.
 *
 * Always returns 16. Soft pure-data tag; does not call malloc.
 * No parent wires.
 */
uint32_t
gj_malloc_align_default_8011(void)
{
	(void)NULL;
	return b8011_default();
}

/*
 * gj_malloc_align_min_8011 - soft minimum allocation alignment.
 *
 * Always returns 8 (LP64 sizeof(void *)).
 */
uint32_t
gj_malloc_align_min_8011(void)
{
	return b8011_min();
}

/*
 * gj_malloc_align_voidptr_8011 - soft sizeof(void *) tag.
 *
 * Always returns 8.
 */
uint32_t
gj_malloc_align_voidptr_8011(void)
{
	return b8011_voidptr();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_malloc_align_default_8011(void)
    __attribute__((alias("gj_malloc_align_default_8011")));

uint32_t __gj_malloc_align_min_8011(void)
    __attribute__((alias("gj_malloc_align_min_8011")));

uint32_t __gj_malloc_align_voidptr_8011(void)
    __attribute__((alias("gj_malloc_align_voidptr_8011")));
