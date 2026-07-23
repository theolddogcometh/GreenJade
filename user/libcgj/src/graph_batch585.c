/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch585: pure hex string → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_hex_u64(const char *s, uint64_t *out);
 *     — Parse a full-string ASCII hexadecimal integer into *out.
 *       Optional "0x" / "0X" prefix is accepted.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_hex_u64  (alias)
 *   __libcgj_batch585_marker = "libcgj-batch585"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch585_marker[] = "libcgj-batch585";

/* ---- helpers ----------------------------------------------------------- */

static int
b585_hex_digit(char c)
{
	if (c >= '0' && c <= '9') {
		return (int)(c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return (int)(c - 'a') + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return (int)(c - 'A') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_hex_u64 — full-string hexadecimal ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   ( "0x" | "0X" )? [0-9a-fA-F]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-hex junk / overflow past UINT64_MAX → -1
 *   "0x" alone (prefix with no digits) → -1
 *   "0", "00", "0x0", "0X00" → 0
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"                    → 0
 *   "ff" / "FF" / "0xff"   → 255
 *   "0xFFFFFFFFFFFFFFFF"   → UINT64_MAX
 *   "0x10000000000000000"  → -1 (overflow)
 *   "" / "x" / "0x" / "1g" / " 1" → -1
 */
int
gj_parse_hex_u64(const char *s, uint64_t *out)
{
	const char *p;
	uint64_t u;
	int nDig;
	int d;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;

	/* Optional 0x / 0X prefix (must be followed by at least one digit). */
	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		p += 2;
	}

	d = b585_hex_digit(*p);
	if (d < 0) {
		return -1;
	}

	u = 0ull;
	nDig = 0;
	while ((d = b585_hex_digit(*p)) >= 0) {
		/* overflow if u would exceed UINT64_MAX after <<4 | d */
		if (u > (UINT64_MAX >> 4)) {
			return -1;
		}
		u = (u << 4) | (uint64_t)d;
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}
	if (*p != '\0') {
		return -1;
	}

	*out = u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_hex_u64(const char *s, uint64_t *out)
    __attribute__((alias("gj_parse_hex_u64")));
