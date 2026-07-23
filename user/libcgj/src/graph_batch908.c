/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch908: median of a sorted uint32 array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_median_sorted(const uint32_t *a, size_t n);
 *     — Lower median of sorted a[0..n): a[(n - 1) / 2].
 *       NULL a or n == 0 → 0.
 *   uint32_t __gj_u32_median_sorted  (alias)
 *   __libcgj_batch908_marker = "libcgj-batch908"
 *
 * Distinct from gj_select_k_i32 (batch310) — pre-sorted uint32 only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch908_marker[] = "libcgj-batch908";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_median_sorted — lower median of a sorted uint32 array.
 *
 * For odd n the middle element is a[n/2] == a[(n-1)/2].
 * For even n the lower of the two central values is chosen
 * (a[n/2 - 1] == a[(n-1)/2]), avoiding a mid-point average that
 * could overflow uint32_t.
 */
uint32_t
gj_u32_median_sorted(const uint32_t *a, size_t n)
{
	if (a == NULL || n == 0u) {
		return 0u;
	}
	return a[(n - 1u) / 2u];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_median_sorted(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_median_sorted")));
