/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8238: memory-region overlap probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mem_overlap_u_8238(uintptr_t a, size_t na,
 *                                  uintptr_t b, size_t nb);
 *     - Return 1 if half-open [a, a+na) intersects [b, b+nb), else 0.
 *       Either length 0 → 0.
 *   uint32_t __gj_mem_overlap_u_8238  (alias)
 *   __libcgj_batch8238_marker = "libcgj-batch8238"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Unique gj_mem_overlap_u_8238 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8238_marker[] = "libcgj-batch8238";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Half-open intervals [uA, uA+na) and [uB, uB+nb) overlap iff
 * uA < uB+nb && uB < uA+na. Pure integer address-space probe.
 */
static uint32_t
b8238_overlap(uintptr_t uA, size_t uNa, uintptr_t uB, size_t uNb)
{
	uintptr_t uAEnd;
	uintptr_t uBEnd;

	if (uNa == 0u || uNb == 0u)
		return 0u;
	uAEnd = uA + (uintptr_t)uNa;
	uBEnd = uB + (uintptr_t)uNb;
	if (uA < uBEnd && uB < uAEnd)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_overlap_u_8238 - 1 if two integer address windows overlap.
 *
 * a, b:   region bases as integer addresses
 * na, nb: region lengths in octets
 *
 * Soft address-space overlap probe; does not read or write memory.
 * No parent wires.
 */
uint32_t
gj_mem_overlap_u_8238(uintptr_t uA, size_t uNa, uintptr_t uB, size_t uNb)
{
	(void)NULL;
	return b8238_overlap(uA, uNa, uB, uNb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mem_overlap_u_8238(uintptr_t uA, size_t uNa,
                                 uintptr_t uB, size_t uNb)
    __attribute__((alias("gj_mem_overlap_u_8238")));
