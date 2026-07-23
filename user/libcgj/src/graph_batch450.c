/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch450: freestanding hexadecimal digit-string check.
 *
 * Surface (unique symbols):
 *   int gj_is_hex_str(const char *s);
 *     — Return 1 if s is a non-empty string of ASCII hex digits
 *       [0-9a-fA-F]+ with an optional leading "0x" / "0X" prefix
 *       (prefix alone is not enough; at least one hex digit required).
 *       Return 0 for NULL, empty, bare "0x", or any non-hex junk.
 *   int __gj_is_hex_str  (alias)
 *   __libcgj_batch450_marker = "libcgj-batch450"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch450_marker[] = "libcgj-batch450";

/* ---- helpers ----------------------------------------------------------- */

static int
b450_is_hex_digit(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch >= 'a' && ch <= 'f') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_hex_str — 1 iff s matches ( "0x" | "0X" )? [0-9a-fA-F]+ fully.
 *
 * Edge cases:
 *   s == NULL → 0
 *   "" / "0x" / "0X" → 0
 *   "0" / "ff" / "FF" / "0xff" / "0X00" → 1
 *   "+1" / "1g" / " 1" / "0x " → 0
 */
int
gj_is_hex_str(const char *s)
{
	const char *p;
	int nDig;

	if (s == NULL) {
		return 0;
	}

	p = s;

	/* Optional 0x / 0X prefix (must be followed by at least one digit). */
	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		p += 2;
	}

	nDig = 0;
	while (*p != '\0') {
		if (!b450_is_hex_digit(*p)) {
			return 0;
		}
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_hex_str(const char *s)
    __attribute__((alias("gj_is_hex_str")));
