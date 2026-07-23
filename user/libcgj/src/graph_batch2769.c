/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2769: three-way sign of int32_t (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_i32_sign_u(int32_t x);
 *     - Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int __gj_i32_sign_u  (alias)
 *   __libcgj_batch2769_marker = "libcgj-batch2769"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_sign_i32 (batch839) / gj_signum_i64 (batch379) / gj_i64_signum
 * (batch2046) - unique gj_i32_sign_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2769_marker[] = "libcgj-batch2769";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sign of i32X as -1, 0, or +1. */
static int
b2769_sign(int32_t i32X)
{
	if (i32X < 0) {
		return -1;
	}
	if (i32X > 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_sign_u - sign of x as -1, 0, or +1.
 *
 * Branch form avoids arithmetic right-shift assumptions and INT32_MIN
 * negation overflow; comparisons are exact for the full int32_t range.
 * Does not call libc.
 */
int
gj_i32_sign_u(int32_t i32X)
{
	(void)NULL;
	return b2769_sign(i32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i32_sign_u(int32_t i32X)
    __attribute__((alias("gj_i32_sign_u")));
