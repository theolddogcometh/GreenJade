/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4282: ASCII hex-digit predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isxdigit_u2(unsigned c);
 *     - Return 1 if c is an ASCII hex digit 0-9 / a-f / A-F; else 0.
 *   int __gj_ascii_isxdigit_u2  (alias)
 *   __libcgj_batch4282_marker = "libcgj-batch4282"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from isxdigit (ctype.c)
 * and span predicates. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4282_marker[] = "libcgj-batch4282";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is 0-9 / a-f / A-F. */
static int
b4282_isxdigit(unsigned uC)
{
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'f') {
		return 1;
	}
	if (uC >= (unsigned)'A' && uC <= (unsigned)'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isxdigit_u2 - 1 iff c is an ASCII hexadecimal digit.
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Edge cases:
 *   '0'..'9' / 'a'..'f' / 'A'..'F' → 1
 *   'g' / 'G' / ' ' / any other → 0
 */
int
gj_ascii_isxdigit_u2(unsigned uC)
{
	(void)NULL;
	return b4282_isxdigit(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isxdigit_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isxdigit_u2")));
