/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1236: XML NameStartChar (ASCII subset).
 *
 * Surface (unique symbols):
 *   int gj_xml_is_name_start(int ch);
 *     — Return 1 if ch may start an XML Name in the ASCII subset:
 *       ':' | [A-Z] | '_' | [a-z]  (XML 1.0 NameStartChar, no
 *       non-ASCII). Else 0. ch outside 0..255 → 0.
 *   int __gj_xml_is_name_start  (alias)
 *   __libcgj_batch1236_marker = "libcgj-batch1236"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1236_marker[] = "libcgj-batch1236";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_is_name_start — ASCII XML NameStartChar.
 */
int
gj_xml_is_name_start(int nCh)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_is_name_start(int nCh)
    __attribute__((alias("gj_xml_is_name_start")));
