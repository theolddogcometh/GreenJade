/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3155: round a pointer up to an alignment (u).
 *
 * Surface (unique symbols):
 *   void *gj_ptr_align_up_u(void *p, size_t align);
 *     - Return the least pointer >= p that is a multiple of align.
 *       align == 0 or 1 → return p. align must be a power of two;
 *       non-pow2 → return p unchanged. On address overflow of the
 *       round-up, returns NULL.
 *   void *__gj_ptr_align_up_u  (alias)
 *   __libcgj_batch3155_marker = "libcgj-batch3155"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_ptr_align_up (batch978) / gj_align_up (batch526) — unique
 * gj_ptr_align_up_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3155_marker[] = "libcgj-batch3155";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (p + align - 1) & ~(align - 1). align <= 1 or non-pow2 → p.
 * Intermediate overflow → NULL.
 */
static void *
b3155_ptr_align_up(void *p, size_t align)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_align_up_u - (p + align - 1) & ~(align - 1) as a pointer.
 *
 * p:     base pointer (NULL is address 0)
 * align: power-of-two alignment (0/1/non-pow2 → return p)
 *
 * Overflow of the intermediate address sum → NULL.
 * No parent wires.
 */
void *
gj_ptr_align_up_u(void *p, size_t align)
{
	(void)NULL;
	return b3155_ptr_align_up(p, align);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_ptr_align_up_u(void *p, size_t align)
    __attribute__((alias("gj_ptr_align_up_u")));
