/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch614: three-way sign of int64_t.
 *
 * Surface (unique symbols):
 *   int gj_i64_sign(int64_t x);
 *     — Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int __gj_i64_sign  (alias)
 *   __libcgj_batch614_marker = "libcgj-batch614"
 *
 * Distinct from gj_signum_i64 / __gj_signum_i64 (graph_batch379) — do
 * not multi-def those symbols here.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch614_marker[] = "libcgj-batch614";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sign — sign of x as -1, 0, or +1.
 *
 * Branch form avoids arithmetic right-shift assumptions and INT64_MIN
 * negation overflow; comparisons are exact for the full int64_t range.
 */
int
gj_i64_sign(int64_t i64X)
{
	if (i64X < 0) {
		return -1;
	}
	if (i64X > 0) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_sign(int64_t i64X)
    __attribute__((alias("gj_i64_sign")));
