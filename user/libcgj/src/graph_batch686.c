/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch686: freestanding binary digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_is_bin_digit(int c);
 *     — Return 1 if c is the ASCII binary digit '0' or '1', else 0.
 *   int __gj_is_bin_digit  (alias)
 *   __libcgj_batch686_marker = "libcgj-batch686"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch686_marker[] = "libcgj-batch686";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_bin_digit — 1 iff c is '0' or '1'.
 *
 * Edge cases:
 *   '0' / '1' → 1
 *   '2'..'9' / letters / EOF(-1) / other → 0
 */
int
gj_is_bin_digit(int c)
{
	return (c == '0' || c == '1') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_bin_digit(int c)
    __attribute__((alias("gj_is_bin_digit")));
