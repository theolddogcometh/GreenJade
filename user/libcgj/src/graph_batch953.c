/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch953: 3x3 matrix trace int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_mat3_trace_i32(const int32_t m[9]);
 *     — Return m[0]+m[4]+m[8]. NULL m → 0.
 *   int32_t __gj_mat3_trace_i32  (alias)
 *   __libcgj_batch953_marker = "libcgj-batch953"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch953_marker[] = "libcgj-batch953";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat3_trace_i32 — sum of diagonal of a row-major 3x3 int32 matrix.
 */
int32_t
gj_mat3_trace_i32(const int32_t *pM)
{
	if (pM == NULL) {
		return 0;
	}
	return pM[0] + pM[4] + pM[8];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_mat3_trace_i32(const int32_t *pM)
    __attribute__((alias("gj_mat3_trace_i32")));
