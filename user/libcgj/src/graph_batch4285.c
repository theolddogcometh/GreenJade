/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4285: ASCII whitespace predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isspace_u2(unsigned c);
 *     - Return 1 if c is space, tab, LF, CR, FF, or VT; else 0.
 *   int __gj_ascii_isspace_u2  (alias)
 *   __libcgj_batch4285_marker = "libcgj-batch4285"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Full C isspace set (SP/HT/LF/VT/
 * FF/CR). Distinct from isspace (ctype.c) and span predicates. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4285_marker[] = "libcgj-batch4285";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 iff uC is SP (0x20), HT (0x09), LF (0x0A), VT (0x0B), FF (0x0C),
 * or CR (0x0D).
 */
static int
b4285_isspace(unsigned uC)
{
	if (uC == (unsigned)' ') {
		return 1;
	}
	if (uC == (unsigned)'\t') {
		return 1;
	}
	if (uC == (unsigned)'\n') {
		return 1;
	}
	if (uC == (unsigned)'\r') {
		return 1;
	}
	if (uC == (unsigned)'\f') {
		return 1;
	}
	if (uC == (unsigned)'\v') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isspace_u2 - 1 iff c is ASCII whitespace (SP/HT/LF/VT/FF/CR).
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Returns 1 for space, tab, LF, CR, FF, VT only; all others → 0.
 * Does not call libc isspace.
 */
int
gj_ascii_isspace_u2(unsigned uC)
{
	(void)NULL;
	return b4285_isspace(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isspace_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isspace_u2")));
