/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1822: XML NameStartChar predicate (_p).
 *
 * Surface (unique symbols):
 *   int gj_xml_name_start_char_p(int ch);
 *     — Return 1 if ch may start an XML Name in the ASCII subset:
 *       ':' | [A-Z] | '_' | [a-z]  (XML 1.0 NameStartChar, no
 *       non-ASCII). Else 0. ch outside 0..255 → 0.
 *   int __gj_xml_name_start_char_p  (alias)
 *   __libcgj_batch1822_marker = "libcgj-batch1822"
 *
 * Distinct from gj_xml_is_name_start (batch1236) — avoid multi-def.
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1822_marker[] = "libcgj-batch1822";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1822_name_start_p(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if (nCh == ':' || nCh == '_') {
		return 1;
	}
	if ((nCh >= 'A' && nCh <= 'Z') || (nCh >= 'a' && nCh <= 'z')) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_name_start_char_p — ASCII XML NameStartChar predicate.
 *
 * Integer-only; no string tables. Use for first-char checks when
 * tokenizing tag or attribute names in a freestanding parser.
 */
int
gj_xml_name_start_char_p(int nCh)
{
	return b1822_name_start_p(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_name_start_char_p(int nCh)
    __attribute__((alias("gj_xml_name_start_char_p")));
