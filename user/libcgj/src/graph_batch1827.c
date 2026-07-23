/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1827: HTML attribute quote predicate.
 *
 * Surface (unique symbols):
 *   int gj_html_attr_quote_ok(int ch);
 *     — Return 1 if ch is a legal HTML attribute value quote delimiter
 *       in the common double/single form: '"' (U+0022) or '\''
 *       (U+0027). Else 0. ch outside 0..255 → 0.
 *   int __gj_html_attr_quote_ok  (alias)
 *   __libcgj_batch1827_marker = "libcgj-batch1827"
 *
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1827_marker[] = "libcgj-batch1827";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1827_attr_quote_ok(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if (nCh == '"' || nCh == '\'') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_html_attr_quote_ok — 1 if ch may open/close a quoted attr value.
 *
 * Unquoted attribute values are out of scope; this only classifies
 * the quote character itself for paired-delimiter scanners.
 */
int
gj_html_attr_quote_ok(int nCh)
{
	return b1827_attr_quote_ok(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_html_attr_quote_ok(int nCh)
    __attribute__((alias("gj_html_attr_quote_ok")));
