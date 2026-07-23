/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8043: buffer-region overlap predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mem_overlap_p_8043(const void *a, const void *b, size_t n);
 *     - Return 1 if [a, a+n) and [b, b+n) overlap in address space
 *       (uintptr order), else 0. n == 0 → 0. Either NULL → 0.
 *       Identical pointers with n > 0 → 1 (same region).
 *   uint32_t __gj_mem_overlap_p_8043  (alias)
 *   __libcgj_batch8043_marker = "libcgj-batch8043"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_mem_overlap_p_8043 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8043_marker[] = "libcgj-batch8043";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Half-open intervals [uA, uA+n) and [uB, uB+n) overlap iff
 * uA < uB+n && uB < uA+n. Uses uintptr for freestanding order.
 */
static uint32_t
b8043_overlap(uintptr_t uA, uintptr_t uB, size_t n)
{
	uintptr_t uAEnd;
	uintptr_t uBEnd;

	if (n == 0u) {
		return 0u;
	}
	uAEnd = uA + (uintptr_t)n;
	uBEnd = uB + (uintptr_t)n;
	if (uA < uBEnd && uB < uAEnd) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_overlap_p_8043 - 1 if two n-byte regions overlap.
 *
 * a, b: region bases (NULL → 0)
 * n:    length of each region in octets
 *
 * Soft address-space overlap probe for memcpy vs memmove selection.
 * Does not read or write the pointed-to memory. No parent wires.
 */
uint32_t
gj_mem_overlap_p_8043(const void *pA, const void *pB, size_t n)
{
	(void)NULL;
	if (n == 0u || pA == NULL || pB == NULL) {
		return 0u;
	}
	return b8043_overlap((uintptr_t)pA, (uintptr_t)pB, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mem_overlap_p_8043(const void *pA, const void *pB, size_t n)
    __attribute__((alias("gj_mem_overlap_p_8043")));
