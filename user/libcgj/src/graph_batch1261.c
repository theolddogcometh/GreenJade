/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1261: PBKDF2 iteration-count range check.
 *
 * Surface (unique symbols):
 *   int gj_pbkdf2_iter_ok(uint32_t iters);
 *     — Return 1 if iters is in the inclusive range [1, 1<<24]
 *       (1..16777216). Else 0.
 *   int __gj_pbkdf2_iter_ok  (alias)
 *   __libcgj_batch1261_marker = "libcgj-batch1261"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1261_marker[] = "libcgj-batch1261";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pbkdf2_iter_ok — 1 if PBKDF2 iteration count is in [1, 1<<24].
 */
int
gj_pbkdf2_iter_ok(uint32_t uIters)
{
	if (uIters < 1u || uIters > (1u << 24)) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pbkdf2_iter_ok(uint32_t uIters)
    __attribute__((alias("gj_pbkdf2_iter_ok")));
