/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4674: C-string character membership (string-
 * measure wave, collision rename).
 *
 * Surface (unique symbols):
 *   int gj_str_contains_char_u2(const char *s, int c);
 *     - Return 1 if (unsigned char)c occurs in the NUL-terminated
 *       string s (payload octets before the terminator). s == NULL → 0.
 *       Terminating NUL is not a match target for c == 0 (payload-only).
 *       Renamed from gj_str_contains_char_u (collision batch3285).
 *   int __gj_str_contains_char_u2  (alias)
 *   __libcgj_batch4674_marker = "libcgj-batch4674"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_contains_char_u (batch3285), gj_str_chr / gj_str_chr_u —
 * unique contains_char_u2 predicate; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4674_marker[] = "libcgj-batch4674";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4674_contains_char(const char *sz, int nC)
{
	unsigned char ch;
	const unsigned char *p;

	if (sz == NULL) {
		return 0;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)(const void *)sz;
	while (*p != (unsigned char)'\0') {
		if (*p == ch) {
			return 1;
		}
		p++;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_contains_char_u2 - 1 if byte c occurs in s, else 0.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Payload-only scan (stops at first NUL). No parent wires.
 */
int
gj_str_contains_char_u2(const char *s, int c)
{
	(void)NULL;
	return b4674_contains_char(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_contains_char_u2(const char *s, int c)
    __attribute__((alias("gj_str_contains_char_u2")));
