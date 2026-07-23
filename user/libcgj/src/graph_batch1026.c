/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1026: wctype-style ASCII range predicate.
 *
 * Surface (unique symbols):
 *   int gj_wctype_is_ascii(int ch);
 *     — Return 1 if ch is in the classic ASCII range 0..127,
 *       else 0. Negative values → 0.
 *   int __gj_wctype_is_ascii  (alias)
 *   __libcgj_batch1026_marker = "libcgj-batch1026"
 *
 * Does NOT redefine isascii / gj_utf8_is_ascii (buffer form).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1026_marker[] = "libcgj-batch1026";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wctype_is_ascii — true when ch is a classic 7-bit ASCII code unit.
 *
 * nCh: character / wide-character-like value
 *
 * Returns 1 for 0 <= nCh <= 127, else 0.
 */
int
gj_wctype_is_ascii(int nCh)
{
	if (nCh < 0) {
		return 0;
	}
	if (nCh > 127) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wctype_is_ascii(int nCh)
    __attribute__((alias("gj_wctype_is_ascii")));
