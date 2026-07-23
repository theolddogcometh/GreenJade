/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3889: XML NameChar (ASCII subset, _u).
 *
 * Surface (unique symbols):
 *   int gj_xml_is_name_char_u(int ch);
 *     - Return 1 if ch may continue an XML Name in the ASCII subset:
 *       NameStartChar | '-' | '.' | [0-9]  (XML 1.0 NameChar without
 *       non-ASCII / combining marks). Else 0. ch outside 0..255 → 0.
 *   int __gj_xml_is_name_char_u  (alias)
 *   __libcgj_batch3889_marker = "libcgj-batch3889"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_xml_is_name_char (batch1237) and gj_xml_is_name_start_u
 * (batch3888) — unique _u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3889_marker[] = "libcgj-batch3889";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3889_is_name_char(int nCh)
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
 * gj_xml_is_name_char_u — ASCII XML NameChar.
 *
 * No parent wires.
 */
int
gj_xml_is_name_char_u(int nCh)
{
	(void)NULL;
	return b3889_is_name_char(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_is_name_char_u(int nCh)
    __attribute__((alias("gj_xml_is_name_char_u")));
