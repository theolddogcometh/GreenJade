/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2107: NUL-terminated printable-ASCII predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_printable_ascii_p(const char *s);
 *     - Return 1 if every octet of the NUL-terminated string s is ASCII
 *       printable (0x20..0x7E inclusive), else 0. s == NULL -> 0.
 *       Empty string ("") is vacuously printable and returns 1.
 *   int __gj_str_printable_ascii_p  (alias)
 *   __libcgj_batch2107_marker = "libcgj-batch2107"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_is_printable_n (batch1201, bounded span) and
 * gj_ascii_is_print_n (batch661) - unique NUL-string _p surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2107_marker[] = "libcgj-batch2107";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2107_is_print(unsigned char u8C)
{
	return (u8C >= 0x20u && u8C <= 0x7eu) ? 1 : 0;
}

static int
b2107_printable_p(const char *sz)
{
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	for (i = 0u; sz[i] != '\0'; i++) {
		if (!b2107_is_print((unsigned char)sz[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_printable_ascii_p - 1 if s is all printable ASCII (or empty).
 *
 * s: NUL-terminated C string (NULL -> 0)
 *
 * Walks until NUL; does not accept embedded NULs as content (NUL ends
 * the scan). Does not call libc.
 */
int
gj_str_printable_ascii_p(const char *s)
{
	return b2107_printable_p(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_printable_ascii_p(const char *s)
    __attribute__((alias("gj_str_printable_ascii_p")));
