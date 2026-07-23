/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch432: freestanding byte-buffer equality.
 *
 * Surface (unique symbols):
 *   int gj_buf_eq(const uint8_t *a, size_t na, const uint8_t *b, size_t nb);
 *     — Return 1 if the two byte ranges are equal (same length and same
 *       content), else 0. Lengths that differ short-circuit to 0. Either
 *       pointer may be NULL only when its corresponding length is 0; a
 *       non-zero length with a NULL pointer yields 0.
 *   int __gj_buf_eq  (alias)
 *   __libcgj_batch432_marker = "libcgj-batch432"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch432_marker[] = "libcgj-batch432";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_eq — constant-structure equality of two bounded byte ranges.
 *
 * a / na: first range (base + length)
 * b / nb: second range (base + length)
 *
 * Returns 1 on equal, 0 on unequal or invalid non-empty NULL base.
 */
int
gj_buf_eq(const uint8_t *pA, size_t cbNa, const uint8_t *pB, size_t cbNb)
{
	size_t i;

	if (cbNa != cbNb) {
		return 0;
	}

	if (cbNa == 0u) {
		return 1;
	}

	if (pA == NULL || pB == NULL) {
		return 0;
	}

	for (i = 0u; i < cbNa; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}

	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_eq(const uint8_t *pA, size_t cbNa, const uint8_t *pB, size_t cbNb)
    __attribute__((alias("gj_buf_eq")));
