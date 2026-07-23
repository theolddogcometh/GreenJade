/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1131: bencode digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_bencode_is_digit(int ch);
 *     — Return 1 if ch is ASCII '0'..'9' (bencode integer/string
 *       length digits). ch outside 0..255 → 0.
 *   int __gj_bencode_is_digit  (alias)
 *   __libcgj_batch1131_marker = "libcgj-batch1131"
 *
 * Distinct from isdigit / locale-aware checks — unique name.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1131_marker[] = "libcgj-batch1131";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bencode_is_digit — 1 if nCh is an ASCII decimal digit.
 */
int
gj_bencode_is_digit(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	return (nCh >= '0' && nCh <= '9') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bencode_is_digit(int nCh)
    __attribute__((alias("gj_bencode_is_digit")));
