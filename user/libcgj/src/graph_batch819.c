/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch819: freestanding leading run-length of a
 * uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_run_length(const uint32_t *a, size_t n);
 *     — Count of consecutive elements equal to a[0] starting at index
 *       0 (the leading equal run). a == NULL or n == 0 → 0. When n > 0
 *       and a non-NULL, result is in [1, n].
 *   size_t __gj_u32_run_length  (alias)
 *   __libcgj_batch819_marker = "libcgj-batch819"
 *
 * Distinct from prefix/delta (batch817/818) — unique gj_u32_run_length
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch819_marker[] = "libcgj-batch819";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_run_length — length of the equal run starting at a[0].
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Scans forward while a[i] == a[0]; stops at first mismatch or end.
 */
size_t
gj_u32_run_length(const uint32_t *pA, size_t n)
{
	size_t i;
	uint32_t u32V;

	if (pA == NULL || n == 0u) {
		return 0u;
	}

	u32V = pA[0];
	i = 1u;
	while (i < n && pA[i] == u32V) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_run_length(const uint32_t *pA, size_t n)
    __attribute__((alias("gj_u32_run_length")));
