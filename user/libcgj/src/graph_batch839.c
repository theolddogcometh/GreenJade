/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch839: three-way sign of int32_t.
 *
 * Surface (unique symbols):
 *   int gj_sign_i32(int32_t x);
 *     — Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int __gj_sign_i32  (alias)
 *   __libcgj_batch839_marker = "libcgj-batch839"
 *
 * Distinct from gj_signum_i64 (batch379) — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch839_marker[] = "libcgj-batch839";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sign_i32 — sign of x as -1, 0, or +1.
 *
 * Branch form avoids arithmetic right-shift assumptions and INT32_MIN
 * negation overflow; comparisons are exact for the full int32_t range.
 */
int
gj_sign_i32(int32_t i32X)
{
	if (i32X < 0) {
		return -1;
	}
	if (i32X > 0) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sign_i32(int32_t i32X)
    __attribute__((alias("gj_sign_i32")));
