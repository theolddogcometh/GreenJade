/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3186: test nondecreasing uint8_t order (_u).
 *
 * Surface (unique symbols):
 *   int gj_buf_is_sorted_u8_u(const uint8_t *a, size_t n);
 *     - Return 1 (OK) if a[0..n) is sorted nondecreasing
 *       (a[i] <= a[i+1]), else 0 (FAIL). a == NULL with n > 1 -> 0;
 *       n <= 1 -> 1 (vacuously sorted). Equal adjacent elements allowed.
 *   int __gj_buf_is_sorted_u8_u  (alias)
 *   __libcgj_batch3186_marker = "libcgj-batch3186"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_buf_is_sorted_u8_u surface only; no multi-def. Distinct from
 * gj_mem_is_sorted_u8 (batch733) and gj_is_sorted_u32 (batch405).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3186_marker[] = "libcgj-batch3186";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3186_is_sorted(const uint8_t *pA, size_t cN)
{
	size_t i;

	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	for (i = 1u; i < cN; i++) {
		if (pA[i] < pA[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_is_sorted_u8_u - 1 if nondecreasing uint8_t order, else 0.
 *
 * Edge cases:
 *   n <= 1 -> 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 -> 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 *
 * Does not call libc. No parent wires.
 */
int
gj_buf_is_sorted_u8_u(const uint8_t *pA, size_t cN)
{
	(void)NULL;
	return b3186_is_sorted(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_is_sorted_u8_u(const uint8_t *pA, size_t cN)
    __attribute__((alias("gj_buf_is_sorted_u8_u")));
