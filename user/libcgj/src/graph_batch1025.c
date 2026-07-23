/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1025: ASCII titlecase of one character.
 *
 * Surface (unique symbols):
 *   int gj_ascii_titlecase_char(int ch);
 *     — Titlecase one character for the ASCII range: a–z → A–Z.
 *       Non-lowercase-ASCII octets are returned unchanged (masked
 *       to 8 bits for non-negative input; negative → unchanged).
 *   int __gj_ascii_titlecase_char  (alias)
 *   __libcgj_batch1025_marker = "libcgj-batch1025"
 *
 * Does NOT redefine toupper / gj_str_map_upper.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1025_marker[] = "libcgj-batch1025";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_titlecase_char — ASCII titlecase of a single character.
 *
 * nCh: character value (typically unsigned char promoted to int)
 *
 * For ASCII, titlecase coincides with uppercase: 'a'..'z' → 'A'..'Z'.
 * Values outside that range are returned as-is. Negative nCh is
 * returned unchanged (no wrap through unsigned conversion).
 */
int
gj_ascii_titlecase_char(int nCh)
{
	if (nCh >= (int)'a' && nCh <= (int)'z') {
		return nCh - ((int)'a' - (int)'A');
	}
	return nCh;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_titlecase_char(int nCh)
    __attribute__((alias("gj_ascii_titlecase_char")));
