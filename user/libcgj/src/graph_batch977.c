/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch977: pointer alignment predicate.
 *
 * Surface (unique symbols):
 *   int gj_ptr_is_aligned(const void *p, size_t align);
 *     — Return 1 if p is aligned to align, else 0.
 *       align == 0 or 1 → 1 (every pointer is byte-aligned).
 *       align must be a power of two; non-pow2 → 0.
 *       p == NULL is treated as address 0 (aligned only for any pow2).
 *   int __gj_ptr_is_aligned  (alias)
 *   __libcgj_batch977_marker = "libcgj-batch977"
 *
 * Unique gj_ptr_is_aligned surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch977_marker[] = "libcgj-batch977";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_is_aligned — test whether p meets alignment align.
 *
 * p:     pointer (NULL is address 0)
 * align: required alignment (0/1 → always true; must be power of two)
 *
 * Returns 1 if aligned, 0 otherwise (including non-pow2 align).
 */
int
gj_ptr_is_aligned(const void *p, size_t align)
{
	uintptr_t uPtr;

	if (align <= 1u) {
		return 1;
	}
	if ((align & (align - 1u)) != 0u) {
		return 0;
	}

	uPtr = (uintptr_t)p;
	return ((uPtr & (align - 1u)) == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ptr_is_aligned(const void *p, size_t align)
    __attribute__((alias("gj_ptr_is_aligned")));
