/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1003: half-open memory-range overlap predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_overlap(const void *a, size_t na, const void *b, size_t nb);
 *     — Return 1 if half-open ranges [a, a+na) and [b, b+nb) overlap in
 *       address space (via uintptr_t), else 0. Empty range (na == 0 or
 *       nb == 0) or NULL base → 0 (no overlap).
 *   int __gj_mem_overlap  (alias)
 *   __libcgj_batch1003_marker = "libcgj-batch1003"
 *
 * Distinct from gj_mem_eq / gj_mem_cmp / gj_mem_chr (batch365/399/400).
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1003_marker[] = "libcgj-batch1003";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_overlap — 1 if [a, a+na) intersects [b, b+nb).
 *
 * a, na: first range base and byte length
 * b, nb: second range base and byte length
 *
 * Overlap test (no empty ranges): ua < ub + nb && ub < ua + na, using
 * uintptr_t address values. Empty or NULL bases never overlap.
 *
 * Note: comparing pointers from unrelated objects is implementation-
 * defined in ISO C; this freestanding helper intentionally reinterprets
 * as uintptr_t for linear address-space overlap checks.
 */
int
gj_mem_overlap(const void *a, size_t na, const void *b, size_t nb)
{
	uintptr_t ua;
	uintptr_t ub;
	uintptr_t uaEnd;
	uintptr_t ubEnd;

	if (a == NULL || b == NULL || na == 0u || nb == 0u) {
		return 0;
	}

	ua = (uintptr_t)a;
	ub = (uintptr_t)b;

	/* Saturating ends: wrap means range covers to top of address space. */
	uaEnd = ua + (uintptr_t)na;
	if (uaEnd < ua) {
		uaEnd = ~(uintptr_t)0;
	}
	ubEnd = ub + (uintptr_t)nb;
	if (ubEnd < ub) {
		ubEnd = ~(uintptr_t)0;
	}

	return (ua < ubEnd && ub < uaEnd) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_overlap(const void *a, size_t na, const void *b, size_t nb)
    __attribute__((alias("gj_mem_overlap")));
