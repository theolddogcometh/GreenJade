/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch404: sum of a uint32_t array (wrapping).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sum_u32(const uint32_t *a, size_t n);
 *     — Return the sum of a[0..n) as uint32_t (modular wrap on overflow).
 *       a == NULL or n == 0 → 0.
 *   uint32_t __gj_sum_u32  (alias)
 *   __libcgj_batch404_marker = "libcgj-batch404"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch404_marker[] = "libcgj-batch404";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sum_u32 — modular sum of a[0..n).
 *
 * Edge cases:
 *   a == NULL → 0
 *   n == 0 → 0
 *   overflow past UINT32_MAX wraps (unsigned arithmetic)
 */
uint32_t
gj_sum_u32(const uint32_t *a, size_t n)
{
	size_t i;
	uint32_t uSum;

	if (a == NULL || n == 0u) {
		return 0u;
	}

	uSum = 0u;
	for (i = 0u; i < n; i++) {
		uSum += a[i];
	}
	return uSum;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sum_u32(const uint32_t *a, size_t n)
    __attribute__((alias("gj_sum_u32")));
