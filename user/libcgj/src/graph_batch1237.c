/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1237: XML NameChar (ASCII subset).
 *
 * Surface (unique symbols):
 *   int gj_xml_is_name_char(int ch);
 *     — Return 1 if ch may continue an XML Name in the ASCII subset:
 *       NameStartChar | '-' | '.' | [0-9]  (XML 1.0 NameChar without
 *       non-ASCII / combining marks). Else 0. ch outside 0..255 → 0.
 *   int __gj_xml_is_name_char  (alias)
 *   __libcgj_batch1237_marker = "libcgj-batch1237"
 *
 * Distinct from gj_xml_is_name_start (batch1236) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1237_marker[] = "libcgj-batch1237";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_is_name_char — ASCII XML NameChar.
 */
int
gj_xml_is_name_char(int nCh)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_is_name_char(int nCh)
    __attribute__((alias("gj_xml_is_name_char")));
