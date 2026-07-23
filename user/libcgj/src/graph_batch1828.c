/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1828: XML CDATA section open predicate.
 *
 * Surface (unique symbols):
 *   int gj_xml_cdata_open_p(const char *s);
 *     — Return 1 if s begins with the nine-byte CDATA open mark
 *       "<![CDATA[" (exact ASCII, case-sensitive per XML). Does not
 *       require a closing "]]>". NULL / shorter / mismatch → 0.
 *   int __gj_xml_cdata_open_p  (alias)
 *   __libcgj_batch1828_marker = "libcgj-batch1828"
 *
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1828_marker[] = "libcgj-batch1828";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1828_cdata_open_p(const char *sz)
{
	static const char szOpen[] = "<![CDATA[";
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	for (i = 0; szOpen[i] != '\0'; i++) {
		if (sz[i] != szOpen[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_cdata_open_p — 1 if s starts with "<![CDATA[".
 *
 * Exact nine-byte match. Suitable as a branch before entering a CDATA
 * copy loop in a freestanding XML scanner.
 */
int
gj_xml_cdata_open_p(const char *sz)
{
	return b1828_cdata_open_p(sz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_cdata_open_p(const char *sz)
    __attribute__((alias("gj_xml_cdata_open_p")));
