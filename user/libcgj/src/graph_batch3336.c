/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3336: remove one uint32_t at an index from a
 * dense list array (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_list_u32_remove_u(uint32_t *a, size_t n, size_t i);
 *     - Erase a[i] by shifting a[i+1..n) one slot left. Returns the new
 *       length (n-1) on success. NULL a, n == 0, or i >= n -> return n
 *       unchanged. The former last slot is left unspecified.
 *   size_t __gj_list_u32_remove_u  (alias)
 *   __libcgj_batch3336_marker = "libcgj-batch3336"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_u32_erase_at (batch1008), gj_u32_remove_value (batch1006), and
 * gj_list_unlink (batch2339) - unique gj_list_u32_remove_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3336_marker[] = "libcgj-batch3336";

/* ---- freestanding helpers ---------------------------------------------- */

/* Erase element at iIdx; return new length or old cN on failure. */
static size_t
b3336_remove(uint32_t *pA, size_t cN, size_t iIdx)
{
	size_t j;

	if (pA == NULL || cN == 0u || iIdx >= cN) {
		return cN;
	}

	for (j = iIdx + 1u; j < cN; j++) {
		pA[j - 1u] = pA[j];
	}
	return cN - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_u32_remove_u - erase element at index i; return new length.
 *
 * a: base of n uint32_t elements (NULL -> return n)
 * n: current element count
 * i: index to erase (must be < n)
 *
 * Copies a[i+1..n) onto a[i..n-1). Returns n-1 on success, n on failure.
 * No parent wires. Does not call libc.
 */
size_t
gj_list_u32_remove_u(uint32_t *a, size_t n, size_t i)
{
	(void)NULL;
	return b3336_remove(a, n, i);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_u32_remove_u(uint32_t *a, size_t n, size_t i)
    __attribute__((alias("gj_list_u32_remove_u")));
