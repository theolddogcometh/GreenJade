/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2108: terminal display-width hint over a span.
 *
 * Surface (unique symbols):
 *   size_t gj_str_width_hint_n(const char *s, size_t n);
 *     - Soft display-column width of s[0..n) under a fixed 8-column tab
 *       stop model: ASCII printable (0x20..0x7E, excluding HT) adds 1;
 *       HT (0x09) advances to the next multiple of 8 columns; all other
 *       octets (C0 controls, DEL, high-bit) add 0. s == NULL or n == 0
 *       -> 0. Not a full Unicode width algorithm - ASCII-only hint.
 *   size_t __gj_str_width_hint_n  (alias)
 *   __libcgj_batch2108_marker = "libcgj-batch2108"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Unique
 * gj_str_width_hint_n surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2108_marker[] = "libcgj-batch2108";

/* ---- freestanding helpers ---------------------------------------------- */

enum { b2108_tab_stop = 8 };

static size_t
b2108_width(const char *sz, size_t cb)
{
	size_t i;
	size_t cCol;
	unsigned char u8C;
	size_t cPad;

	if (sz == NULL || cb == 0u) {
		return 0u;
	}

	cCol = 0u;
	for (i = 0u; i < cb; i++) {
		u8C = (unsigned char)sz[i];
		if (u8C == (unsigned char)'\t') {
			/* Advance to next multiple of tab stop. */
			cPad = (size_t)b2108_tab_stop
			    - (cCol % (size_t)b2108_tab_stop);
			if (cPad == 0u) {
				cPad = (size_t)b2108_tab_stop;
			}
			cCol += cPad;
		} else if (u8C >= 0x20u && u8C <= 0x7eu) {
			cCol++;
		}
		/* else: control / DEL / high-bit -> width 0 */
	}
	return cCol;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_width_hint_n - soft terminal column width of s[0..n).
 *
 * s: buffer of n octets (NULL -> 0)
 * n: span length
 *
 * Tab stops every 8 columns (0-based). Printable ASCII (ex. HT) = 1.
 * Does not call libc.
 */
size_t
gj_str_width_hint_n(const char *s, size_t n)
{
	return b2108_width(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_width_hint_n(const char *s, size_t n)
    __attribute__((alias("gj_str_width_hint_n")));
