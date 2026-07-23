/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2881: swap two uint64_t array slots (_u).
 *
 * Surface (unique symbols):
 *   void gj_arr_u64_swap_u(uint64_t *a, size_t n, size_t i, size_t j);
 *     - Exchange a[i] with a[j] when both indices are in [0, n). NULL a,
 *       n == 0, out-of-range index, or i == j → no-op.
 *   void __gj_arr_u64_swap_u  (alias)
 *   __libcgj_batch2881_marker = "libcgj-batch2881"
 *
 * Exclusive sort/search helpers wave (2881-2890). Distinct from
 * gj_bswap64 (batch417 octet reverse) and gj_reverse_u32 (batch401 full
 * reverse) — unique gj_arr_u64_swap_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2881_marker[] = "libcgj-batch2881";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap pA[i] and pA[j]; caller guarantees valid distinct indices. */
static void
b2881_swap(uint64_t *pA, size_t i, size_t j)
{
	uint64_t u64T;

	u64T = pA[i];
	pA[i] = pA[j];
	pA[j] = u64T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_swap_u - exchange two slots in a bounded uint64_t array.
 *
 * a: base of n uint64_t elements (NULL → no-op)
 * n: element count
 * i: first index
 * j: second index
 *
 * When a is non-NULL, i < n, j < n, and i != j, swaps a[i] with a[j].
 * Otherwise does nothing. Mutates only those two slots.
 */
void
gj_arr_u64_swap_u(uint64_t *pA, size_t cN, size_t i, size_t j)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return;
	}
	if (i >= cN || j >= cN) {
		return;
	}
	if (i == j) {
		return;
	}
	b2881_swap(pA, i, j);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u64_swap_u(uint64_t *pA, size_t cN, size_t i, size_t j)
    __attribute__((alias("gj_arr_u64_swap_u")));
