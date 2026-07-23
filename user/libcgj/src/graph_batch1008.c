/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1008: erase one uint32_t at an index.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_erase_at(uint32_t *a, size_t n, size_t i);
 *     — Erase a[i] by shifting a[i+1..n) one slot left. Returns the new
 *       length (n-1) on success. NULL a, n == 0, or i >= n → return n
 *       unchanged. The former last slot is left unspecified.
 *   size_t __gj_u32_erase_at  (alias)
 *   __libcgj_batch1008_marker = "libcgj-batch1008"
 *
 * Distinct from gj_u32_remove_value (batch1006, value-based compact) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1008_marker[] = "libcgj-batch1008";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_erase_at — erase element at index i; return new length or old n.
 *
 * a: base of n uint32_t elements (NULL → return n)
 * n: current element count
 * i: index to erase (must be < n)
 *
 * Copies a[i+1..n) onto a[i..n-1). Returns n-1 on success, n on failure.
 */
size_t
gj_u32_erase_at(uint32_t *a, size_t n, size_t i)
{
	size_t j;

	if (a == NULL || n == 0u || i >= n) {
		return n;
	}

	for (j = i + 1u; j < n; j++) {
		a[j - 1u] = a[j];
	}
	return n - 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_erase_at(uint32_t *a, size_t n, size_t i)
    __attribute__((alias("gj_u32_erase_at")));
