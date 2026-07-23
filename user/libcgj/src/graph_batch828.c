/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch828: ASCII digit character value.
 *
 * Surface (unique symbols):
 *   int gj_digit_val(int c);
 *     — '0'..'9' → 0..9; otherwise -1. Accepts int so callers may pass
 *       signed char or unsigned char without cast surprises.
 *   int __gj_digit_val  (alias)
 *   __libcgj_batch828_marker = "libcgj-batch828"
 *
 * Distinct from gj_digit_count_u64 / gj_digit_sum_u64 — char→value only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch828_marker[] = "libcgj-batch828";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digit_val — decimal value of ASCII digit character, else -1.
 */
int
gj_digit_val(int nC)
{
	if (nC >= (int)'0' && nC <= (int)'9') {
		return nC - (int)'0';
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_digit_val(int nC)
    __attribute__((alias("gj_digit_val")));
