/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2034: floor unsigned 64-bit division.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_floor(uint64_t a, uint64_t b);
 *     — floor(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b (including 0).
 *   uint64_t __gj_u64_div_floor  (alias)
 *   __libcgj_batch2034_marker = "libcgj-batch2034"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). For unsigned
 * operands floor division matches C truncating division.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2034_marker[] = "libcgj-batch2034";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a / b); b == 0 yields 0. */
static uint64_t
b2034_div_floor(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A / u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_floor — floor divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 */
uint64_t
gj_u64_div_floor(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2034_div_floor(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_floor(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_div_floor")));
