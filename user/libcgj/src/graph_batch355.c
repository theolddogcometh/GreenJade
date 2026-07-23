/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch355: in-place ASCII case fold for C strings.
 *
 * Surface (unique symbols):
 *   void gj_ascii_tolower(char *s);
 *     — Convert each A..Z octet in the NUL-terminated string s to a..z
 *       in place. Non-letters and non-ASCII bytes are left unchanged.
 *       No-op when s is NULL.
 *   void gj_ascii_toupper(char *s);
 *     — Convert each a..z octet in the NUL-terminated string s to A..Z
 *       in place. Non-letters and non-ASCII bytes are left unchanged.
 *       No-op when s is NULL.
 *   void __gj_ascii_tolower / void __gj_ascii_toupper  (aliases)
 *   __libcgj_batch355_marker = "libcgj-batch355"
 *
 * Distinct from ctype tolower/toupper (single int code unit, locale-aware
 * graph via ctype.c / graph_batch15.c / graph_batch17.c / graph_batch26.c)
 * and from locale *_l / __ctype_*_loc tables. This TU does not define
 * tolower, toupper, tolower_l, or toupper_l — avoids multi-def with the
 * existing libc graph. Uses only A..Z / a..z ASCII range arithmetic
 * (static helpers, not exported).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch355_marker[] = "libcgj-batch355";

/* ---- freestanding helpers (not exported; not tolower/toupper) ---------- */

/* Map one unsigned octet: A..Z → a..z; else identity. */
static unsigned char
b355_to_lower(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return u8Ch;
}

/* Map one unsigned octet: a..z → A..Z; else identity. */
static unsigned char
b355_to_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_tolower — fold ASCII uppercase letters in place.
 *
 * s: mutable NUL-terminated C string (NULL → no-op)
 *
 * Walks until the first 0 byte. Only A..Z (0x41..0x5A) are changed.
 */
void
gj_ascii_tolower(char *s)
{
	unsigned char *pCh;

	if (s == NULL) {
		return;
	}

	pCh = (unsigned char *)s;
	while (*pCh != 0u) {
		*pCh = b355_to_lower(*pCh);
		pCh++;
	}
}

/*
 * gj_ascii_toupper — fold ASCII lowercase letters in place.
 *
 * s: mutable NUL-terminated C string (NULL → no-op)
 *
 * Walks until the first 0 byte. Only a..z (0x61..0x7A) are changed.
 */
void
gj_ascii_toupper(char *s)
{
	unsigned char *pCh;

	if (s == NULL) {
		return;
	}

	pCh = (unsigned char *)s;
	while (*pCh != 0u) {
		*pCh = b355_to_upper(*pCh);
		pCh++;
	}
}

void __gj_ascii_tolower(char *s)
    __attribute__((alias("gj_ascii_tolower")));

void __gj_ascii_toupper(char *s)
    __attribute__((alias("gj_ascii_toupper")));
