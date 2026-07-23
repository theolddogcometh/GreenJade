/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2079: base64 alphabet character predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_base64_char(int c);
 *     — Return 1 if c is a standard base64 alphabet octet:
 *       A-Z / a-z / 0-9 / '+' / '/' / '=' (padding).
 *       c outside 0..255 → 0. Other octets → 0.
 *   int __gj_str_is_base64_char  (alias)
 *   __libcgj_batch2079_marker = "libcgj-batch2079"
 *
 * Str encode exclusive wave (2071–2080). Single-char class only —
 * avoid multi-def with full base64 encode/decode TUs.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2079_marker[] = "libcgj-batch2079";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2079_is_b64 — 1 if u8C is in the standard base64 alphabet incl. '='.
 */
static int
b2079_is_b64(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C == (unsigned char)'+' || u8C == (unsigned char)'/' ||
	    u8C == (unsigned char)'=') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_base64_char — 1 if c is a standard base64 alphabet octet.
 *
 * Alphabet: A-Za-z0-9+/= . Values outside 0..255 return 0.
 */
int
gj_str_is_base64_char(int nC)
{
	(void)NULL;
	if (nC < 0 || nC > 255) {
		return 0;
	}
	return b2079_is_b64((unsigned char)nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_base64_char(int nC)
    __attribute__((alias("gj_str_is_base64_char")));
