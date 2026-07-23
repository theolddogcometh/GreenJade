/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1006: compact-remove all occurrences of a
 * value from a uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_remove_value(uint32_t *a, size_t n, uint32_t v);
 *     — Remove every element equal to v from a[0..n), compacting the
 *       survivors to the front in relative order. Returns the new
 *       length. NULL a → 0; n == 0 → 0. Tail slots past the new length
 *       are left unspecified (not cleared).
 *   size_t __gj_u32_remove_value  (alias)
 *   __libcgj_batch1006_marker = "libcgj-batch1006"
 *
 * Distinct from gj_u32_unique_sorted (batch666) and other u32 array
 * surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1006_marker[] = "libcgj-batch1006";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_remove_value — compact-remove all elements equal to v.
 *
 * a: base of n uint32_t elements (NULL → return 0)
 * n: element count
 * v: value to strip
 *
 * Single write-pointer pass: for each a[i] != v, write it to a[w++] and
 * advance. Returns w (new length in 0..n). Relative order of kept
 * elements is preserved. Does not zero the discarded tail.
 */
size_t
gj_u32_remove_value(uint32_t *a, size_t n, uint32_t v)
{
	size_t i;
	size_t w;

	if (a == NULL || n == 0u) {
		return 0u;
	}

	w = 0u;
	for (i = 0u; i < n; i++) {
		if (a[i] != v) {
			a[w] = a[i];
			w++;
		}
	}
	return w;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_remove_value(uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_remove_value")));
