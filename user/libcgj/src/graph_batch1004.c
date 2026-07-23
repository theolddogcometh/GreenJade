/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1004: half-open memory-range disjoint predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_disjoint(const void *a, size_t na, const void *b, size_t nb);
 *     — Return 1 if half-open ranges [a, a+na) and [b, b+nb) do not
 *       overlap, else 0. Empty range or NULL base → 1 (vacuously
 *       disjoint). Complement of gj_mem_overlap (batch1003).
 *   int __gj_mem_disjoint  (alias)
 *   __libcgj_batch1004_marker = "libcgj-batch1004"
 *
 * Does NOT redefine gj_mem_overlap (batch1003) — avoid multi-def.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1004_marker[] = "libcgj-batch1004";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_disjoint — 1 if [a, a+na) and [b, b+nb) do not intersect.
 *
 * a, na: first range base and byte length
 * b, nb: second range base and byte length
 *
 * Empty (na == 0 or nb == 0) or NULL base → 1. Otherwise the ranges are
 * disjoint when NOT (ua < ubEnd && ub < uaEnd).
 */
int
gj_mem_disjoint(const void *a, size_t na, const void *b, size_t nb)
{
	uintptr_t ua;
	uintptr_t ub;
	uintptr_t uaEnd;
	uintptr_t ubEnd;

	if (a == NULL || b == NULL || na == 0u || nb == 0u) {
		return 1;
	}

	ua = (uintptr_t)a;
	ub = (uintptr_t)b;

	uaEnd = ua + (uintptr_t)na;
	if (uaEnd < ua) {
		uaEnd = ~(uintptr_t)0;
	}
	ubEnd = ub + (uintptr_t)nb;
	if (ubEnd < ub) {
		ubEnd = ~(uintptr_t)0;
	}

	return (ua < ubEnd && ub < uaEnd) ? 0 : 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_disjoint(const void *a, size_t na, const void *b, size_t nb)
    __attribute__((alias("gj_mem_disjoint")));
