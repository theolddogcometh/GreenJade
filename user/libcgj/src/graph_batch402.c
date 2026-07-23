/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch402: fill a uint32_t array with a constant.
 *
 * Surface (unique symbols):
 *   void gj_fill_u32(uint32_t *a, size_t n, uint32_t v);
 *     — Write v into each of the first n elements of a.
 *       a == NULL or n == 0 → no-op.
 *   void __gj_fill_u32  (alias)
 *   __libcgj_batch402_marker = "libcgj-batch402"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch402_marker[] = "libcgj-batch402";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fill_u32 — set a[i] = v for i in [0, n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n == 0 → no-op
 */
void
gj_fill_u32(uint32_t *a, size_t n, uint32_t v)
{
	size_t i;

	if (a == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		a[i] = v;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_fill_u32(uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_fill_u32")));
