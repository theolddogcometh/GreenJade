/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3717: full-string hex string → uint64_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_hex_u(const char *s, uint64_t *out);
 *     - Parse a full-string ASCII hexadecimal integer into *out.
 *       Optional "0x" / "0X" prefix is accepted.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_u64_hex_u  (alias)
 *   __libcgj_batch3717_marker = "libcgj-batch3717"
 *
 * Exclusive continuum CREATE-ONLY (3711-3720). Distinct from
 * gj_parse_u64_hex (batch392), gj_parse_u64_hex_n (batch1533),
 * gj_parse_u64_hex_full (batch2483) — unique _u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3717_marker[] = "libcgj-batch3717";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3717_hex_digit — value 0..15 for hex char, or -1 if not hex.
 */
static int
b3717_hex_digit(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_hex_u — full-string hexadecimal ASCII → uint64_t.
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
gj_parse_u64_hex_u(const char *s, uint64_t *out)
{
	const char *p;
	uint64_t u;
	int nDig;
	int d;

	(void)NULL;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;

	/* Optional 0x / 0X prefix (must be followed by at least one digit). */
	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		p += 2;
	}

	d = b3717_hex_digit(*p);
	if (d < 0) {
		return -1;
	}

	u = 0ull;
	nDig = 0;
	while ((d = b3717_hex_digit(*p)) >= 0) {
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

int __gj_parse_u64_hex_u(const char *s, uint64_t *out)
    __attribute__((alias("gj_parse_u64_hex_u")));
