/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch906: in-place ASCII uppercase map.
 *
 * Surface (unique symbols):
 *   char *gj_str_map_upper(char *s);
 *     — Convert every ASCII lowercase letter a–z in s to A–Z, in place.
 *       Non-letters are left unchanged. Returns s. NULL s → NULL.
 *   char *__gj_str_map_upper  (alias)
 *   __libcgj_batch906_marker = "libcgj-batch906"
 *
 * Does NOT redefine toupper / toupper_l — unique gj_ surface only.
 * Pair of the ASCII lowercase mapper (batch905).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch906_marker[] = "libcgj-batch906";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_map_upper — ASCII a–z → A–Z in place.
 *
 * Only the 26 lowercase Latin letters are mapped. All other octets,
 * including already-uppercase and non-ASCII, are left as-is.
 */
char *
gj_str_map_upper(char *s)
{
	size_t i;

	if (s == NULL) {
		return NULL;
	}

	for (i = 0u; s[i] != '\0'; i++) {
		unsigned char ch = (unsigned char)s[i];

		if (ch >= (unsigned char)'a' && ch <= (unsigned char)'z') {
			s[i] = (char)(ch - (unsigned char)('a' - 'A'));
		}
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_map_upper(char *s)
    __attribute__((alias("gj_str_map_upper")));
