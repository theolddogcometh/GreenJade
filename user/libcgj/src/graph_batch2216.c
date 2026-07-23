/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2216: freestanding half-open overlap predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_overlap_p(const void *a, size_t na, const void *b,
 *                        size_t nb);
 *     - Return 1 if half-open ranges [a, a+na) and [b, b+nb) overlap in
 *       address space (via uintptr_t), else 0. Empty range (na == 0 or
 *       nb == 0) or NULL base -> 0 (no overlap).
 *   int __gj_mem_overlap_p  (alias)
 *   __libcgj_batch2216_marker = "libcgj-batch2216"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from
 * gj_mem_overlap (batch1003) - unique gj_mem_overlap_p surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2216_marker[] = "libcgj-batch2216";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating end address: ua + cb, or all-ones on wrap. */
static uintptr_t
b2216_end(uintptr_t uBase, size_t cb)
{
	uintptr_t uEnd;

	uEnd = uBase + (uintptr_t)cb;
	if (uEnd < uBase) {
		return ~(uintptr_t)0;
	}
	return uEnd;
}

/* 1 if [uA, uAEnd) intersects [uB, uBEnd). Ends are exclusive. */
static int
b2216_overlap_p(uintptr_t uA, uintptr_t uAEnd, uintptr_t uB,
    uintptr_t uBEnd)
{
	if (uA < uBEnd && uB < uAEnd) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_overlap_p - 1 if [a, a+na) intersects [b, b+nb).
 *
 * a, na: first range base and byte length
 * b, nb: second range base and byte length
 *
 * Overlap test uses uintptr_t linear addresses. Empty or NULL bases
 * never overlap. Comparing pointers from unrelated objects is
 * implementation-defined in ISO C; this freestanding helper
 * intentionally reinterprets as uintptr_t for linear address checks.
 */
int
gj_mem_overlap_p(const void *a, size_t na, const void *b, size_t nb)
{
	uintptr_t uA;
	uintptr_t uB;
	uintptr_t uAEnd;
	uintptr_t uBEnd;

	(void)NULL;
	if (a == NULL || b == NULL || na == 0u || nb == 0u) {
		return 0;
	}

	uA = (uintptr_t)a;
	uB = (uintptr_t)b;
	uAEnd = b2216_end(uA, na);
	uBEnd = b2216_end(uB, nb);
	return b2216_overlap_p(uA, uAEnd, uB, uBEnd);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_overlap_p(const void *a, size_t na, const void *b, size_t nb)
    __attribute__((alias("gj_mem_overlap_p")));
