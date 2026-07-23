/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch505: length-bounded in-place ASCII case fold.
 *
 * Surface (unique symbols):
 *   void gj_ascii_tolower_n(char *s, size_t n);
 *     — Convert each A..Z octet in the first n bytes of s to a..z
 *       in place. Non-letters and non-ASCII bytes are left unchanged.
 *       Does not stop at NUL; does not require or write a terminator.
 *       No-op when s is NULL or n is 0.
 *   void gj_ascii_toupper_n(char *s, size_t n);
 *     — Convert each a..z octet in the first n bytes of s to A..Z
 *       in place. Non-letters and non-ASCII bytes are left unchanged.
 *       Does not stop at NUL; does not require or write a terminator.
 *       No-op when s is NULL or n is 0.
 *   void __gj_ascii_tolower_n / void __gj_ascii_toupper_n  (aliases)
 *   __libcgj_batch505_marker = "libcgj-batch505"
 *
 * Distinct from gj_ascii_tolower / gj_ascii_toupper (NUL-terminated walk
 * in graph_batch355.c) and from ctype tolower/toupper (single int code
 * unit, locale-aware graph via ctype.c / graph_batch15/17/26). This TU
 * does not define gj_ascii_tolower, gj_ascii_toupper, tolower, or
 * toupper — avoids multi-def with the existing libc graph. Uses only
 * A..Z / a..z ASCII range arithmetic (static helpers, not exported).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch505_marker[] = "libcgj-batch505";

/* ---- freestanding helpers (not exported; not tolower/toupper) ---------- */

/* Map one unsigned octet: A..Z → a..z; else identity. */
static unsigned char
b505_to_lower(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return u8Ch;
}

/* Map one unsigned octet: a..z → A..Z; else identity. */
static unsigned char
b505_to_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_tolower_n — fold ASCII uppercase letters in a fixed span.
 *
 * s: mutable buffer of at least n bytes (NULL → no-op)
 * n: number of octets to process (0 → no-op)
 *
 * Walks exactly n bytes (NUL is not a stop). Only A..Z (0x41..0x5A)
 * are changed.
 */
void
gj_ascii_tolower_n(char *s, size_t n)
{
	unsigned char *pCh;
	size_t iByte;

	if (s == NULL || n == 0u) {
		return;
	}

	pCh = (unsigned char *)s;
	for (iByte = 0u; iByte < n; iByte++) {
		pCh[iByte] = b505_to_lower(pCh[iByte]);
	}
}

/*
 * gj_ascii_toupper_n — fold ASCII lowercase letters in a fixed span.
 *
 * s: mutable buffer of at least n bytes (NULL → no-op)
 * n: number of octets to process (0 → no-op)
 *
 * Walks exactly n bytes (NUL is not a stop). Only a..z (0x61..0x7A)
 * are changed.
 */
void
gj_ascii_toupper_n(char *s, size_t n)
{
	unsigned char *pCh;
	size_t iByte;

	if (s == NULL || n == 0u) {
		return;
	}

	pCh = (unsigned char *)s;
	for (iByte = 0u; iByte < n; iByte++) {
		pCh[iByte] = b505_to_upper(pCh[iByte]);
	}
}

void __gj_ascii_tolower_n(char *s, size_t n)
    __attribute__((alias("gj_ascii_tolower_n")));

void __gj_ascii_toupper_n(char *s, size_t n)
    __attribute__((alias("gj_ascii_toupper_n")));
