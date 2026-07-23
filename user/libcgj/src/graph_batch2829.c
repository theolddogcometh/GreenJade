/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2829: RFC 3986 percent-encode byte need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_url_encode_need_u(const char *s);
 *     - Return octets needed to percent-encode the NUL-terminated
 *       string s under RFC 3986 unreserved rules (including the
 *       trailing NUL). Unreserved octets (ALPHA / DIGIT / "-" / "." /
 *       "_" / "~") expand to 1; all other octets expand to 3 ("%HH").
 *       NULL s → 0. Empty string → 1 (just the trailing NUL).
 *   size_t __gj_url_encode_need_u  (alias)
 *   __libcgj_batch2829_marker = "libcgj-batch2829"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_url_encode
 * (batch103 write + errno), gj_pct_encode (batch273 buffer), and
 * gj_str_url_needs_encode_p (batch2071 single-char predicate) —
 * length-only surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2829_marker[] = "libcgj-batch2829";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2829_is_unreserved — RFC 3986 §2.3 unreserved set:
 *   ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
static int
b2829_is_unreserved(unsigned char u8C)
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
	if (u8C == (unsigned char)'-' || u8C == (unsigned char)'.' ||
	    u8C == (unsigned char)'_' || u8C == (unsigned char)'~') {
		return 1;
	}
	return 0;
}

/*
 * Bytes needed (incl. trailing NUL) to percent-encode sz.
 * NULL → 0. Empty → 1.
 */
static size_t
b2829_encode_need(const char *sz)
{
	size_t cbOut;
	size_t i;

	if (sz == NULL) {
		return 0u;
	}

	cbOut = 0u;
	for (i = 0u; sz[i] != '\0'; i++) {
		if (b2829_is_unreserved((unsigned char)sz[i])) {
			cbOut = cbOut + 1u;
		} else {
			cbOut = cbOut + 3u; /* %HH */
		}
	}
	return cbOut + 1u; /* trailing NUL */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_encode_need_u - bytes needed (incl. NUL) for percent-encoding s.
 *
 * s: NUL-terminated input (NULL → 0)
 *
 * Unreserved octets pass through as one byte; all others become three
 * ("%HH"). Space expands to %20 (3 octets), not '+'. Length-only; does
 * not write a buffer or call libc.
 *
 * Examples (return includes trailing NUL):
 *   "abc"  → 4
 *   "a b"  → 6  ("a%20b" + NUL)
 *   ""     → 1
 *   NULL   → 0
 */
size_t
gj_url_encode_need_u(const char *sz)
{
	(void)NULL;
	return b2829_encode_need(sz);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_url_encode_need_u(const char *sz)
    __attribute__((alias("gj_url_encode_need_u")));
