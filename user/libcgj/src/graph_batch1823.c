/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1823: XML NameChar predicate (_p).
 *
 * Surface (unique symbols):
 *   int gj_xml_name_char_p(int ch);
 *     — Return 1 if ch may continue an XML Name in the ASCII subset:
 *       NameStartChar | '-' | '.' | [0-9]  (XML 1.0 NameChar without
 *       non-ASCII / combining marks). Else 0. ch outside 0..255 → 0.
 *   int __gj_xml_name_char_p  (alias)
 *   __libcgj_batch1823_marker = "libcgj-batch1823"
 *
 * Distinct from gj_xml_is_name_char (batch1237) — avoid multi-def.
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1823_marker[] = "libcgj-batch1823";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1823_name_char_p(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if (nCh == ':' || nCh == '_' || nCh == '-' || nCh == '.') {
		return 1;
	}
	if ((nCh >= 'A' && nCh <= 'Z') || (nCh >= 'a' && nCh <= 'z')) {
		return 1;
	}
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_name_char_p — ASCII XML NameChar predicate.
 *
 * Companion to gj_xml_name_start_char_p (batch1822). Integer-only.
 */
int
gj_xml_name_char_p(int nCh)
{
	return b1823_name_char_p(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_name_char_p(int nCh)
    __attribute__((alias("gj_xml_name_char_p")));
