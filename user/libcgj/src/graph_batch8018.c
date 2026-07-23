/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8018: pointer alignment stubs (integer).
 *
 * Surface (unique symbols):
 *   uint32_t   gj_ptr_is_aligned_8018(uintptr_t p, size_t align);
 *     - Return 1 if p is aligned to align (power of two); else 0.
 *       Invalid align → 0.
 *   uintptr_t  gj_ptr_align_up_8018(uintptr_t p, size_t align);
 *     - Round pointer value up to alignment; invalid/overflow → 0.
 *   uintptr_t  gj_ptr_align_down_8018(uintptr_t p, size_t align);
 *     - Round pointer value down to alignment; invalid align → 0.
 *   __gj_ptr_*_8018  (aliases)
 *   __libcgj_batch8018_marker = "libcgj-batch8018"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_ptr_*_8018 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8018_marker[] = "libcgj-batch8018";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8018_is_pow2(size_t uAlign)
{
	if (uAlign == 0u) {
		return 0u;
	}
	return ((uAlign & (uAlign - 1u)) == 0u) ? 1u : 0u;
}

static uint32_t
b8018_is_aligned(uintptr_t uP, size_t uAlign)
{
	uintptr_t uMask;

	if (b8018_is_pow2(uAlign) == 0u) {
		return 0u;
	}
	uMask = (uintptr_t)uAlign - (uintptr_t)1u;
	return ((uP & uMask) == (uintptr_t)0u) ? 1u : 0u;
}

static uintptr_t
b8018_align_up(uintptr_t uP, size_t uAlign)
{
	uintptr_t uMask;
	uintptr_t uSum;

	if (b8018_is_pow2(uAlign) == 0u) {
		return (uintptr_t)0u;
	}
	uMask = (uintptr_t)uAlign - (uintptr_t)1u;
	uSum = uP + uMask;
	if (uSum < uP) {
		return (uintptr_t)0u;
	}
	return uSum & ~uMask;
}

static uintptr_t
b8018_align_down(uintptr_t uP, size_t uAlign)
{
	uintptr_t uMask;

	if (b8018_is_pow2(uAlign) == 0u) {
		return (uintptr_t)0u;
	}
	uMask = (uintptr_t)uAlign - (uintptr_t)1u;
	return uP & ~uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_is_aligned_8018 - soft pointer alignment predicate.
 *
 * p:     address as integer
 * align: power-of-two alignment
 *
 * Returns 1 if aligned; else 0. Soft pure-data; no dereference.
 * No parent wires.
 */
uint32_t
gj_ptr_is_aligned_8018(uintptr_t p, size_t align)
{
	(void)NULL;
	return b8018_is_aligned(p, align);
}

/*
 * gj_ptr_align_up_8018 - soft round-up of pointer value.
 *
 * Returns aligned uintptr_t, or 0 on invalid align / overflow.
 */
uintptr_t
gj_ptr_align_up_8018(uintptr_t p, size_t align)
{
	return b8018_align_up(p, align);
}

/*
 * gj_ptr_align_down_8018 - soft round-down of pointer value.
 *
 * Returns aligned uintptr_t, or 0 on invalid align.
 */
uintptr_t
gj_ptr_align_down_8018(uintptr_t p, size_t align)
{
	return b8018_align_down(p, align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_ptr_is_aligned_8018(uintptr_t p, size_t align)
    __attribute__((alias("gj_ptr_is_aligned_8018")));

uintptr_t __gj_ptr_align_up_8018(uintptr_t p, size_t align)
    __attribute__((alias("gj_ptr_align_up_8018")));

uintptr_t __gj_ptr_align_down_8018(uintptr_t p, size_t align)
    __attribute__((alias("gj_ptr_align_down_8018")));
