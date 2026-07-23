/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch978: round a pointer up to an alignment.
 *
 * Surface (unique symbols):
 *   void *gj_ptr_align_up(void *p, size_t align);
 *     — Return the least pointer >= p that is a multiple of align.
 *       align == 0 or 1 → return p. align must be a power of two;
 *       non-pow2 → return p unchanged. On address overflow of the
 *       round-up, returns NULL.
 *   void *__gj_ptr_align_up  (alias)
 *   __libcgj_batch978_marker = "libcgj-batch978"
 *
 * Distinct from gj_align_up (batch526, uint64_t value) and
 * gj_ptr_is_aligned (batch977). Unique gj_ptr_align_up surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch978_marker[] = "libcgj-batch978";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_align_up — (p + align - 1) & ~(align - 1) as a pointer.
 *
 * p:     base pointer (NULL is address 0)
 * align: power-of-two alignment (0/1/non-pow2 → return p)
 *
 * Overflow of the intermediate address sum → NULL.
 */
void *
gj_ptr_align_up(void *p, size_t align)
{
	uintptr_t uPtr;
	uintptr_t uMask;
	uintptr_t uAdj;

	if (align <= 1u || (align & (align - 1u)) != 0u) {
		return p;
	}

	uPtr = (uintptr_t)p;
	uMask = (uintptr_t)align - 1u;

	if (uPtr > (UINTPTR_MAX - uMask)) {
		return NULL;
	}

	uAdj = (uPtr + uMask) & ~uMask;
	return (void *)uAdj;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_ptr_align_up(void *p, size_t align)
    __attribute__((alias("gj_ptr_align_up")));
