/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch905: in-place ASCII lowercase map.
 *
 * Surface (unique symbols):
 *   char *gj_str_map_lower(char *s);
 *     — Convert every ASCII uppercase letter A–Z in s to a–z, in place.
 *       Non-letters are left unchanged. Returns s. NULL s → NULL.
 *   char *__gj_str_map_lower  (alias)
 *   __libcgj_batch905_marker = "libcgj-batch905"
 *
 * Does NOT redefine tolower / tolower_l — unique gj_ surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch905_marker[] = "libcgj-batch905";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_map_lower — ASCII A–Z → a–z in place.
 *
 * Only the 26 uppercase Latin letters are mapped (ch | 0x20). All other
 * octets, including already-lowercase and non-ASCII, are left as-is.
 */
char *
gj_str_map_lower(char *s)
{
	size_t i;

	if (s == NULL) {
		return NULL;
	}

	for (i = 0u; s[i] != '\0'; i++) {
		unsigned char ch = (unsigned char)s[i];

		if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
			s[i] = (char)(ch + (unsigned char)('a' - 'A'));
		}
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_map_lower(char *s)
    __attribute__((alias("gj_str_map_lower")));
