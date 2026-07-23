/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch657: length-bounded ASCII digit span predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_is_digit_n(const char *s, size_t n);
 *     — Return 1 if every octet in s[0..n) is an ASCII decimal digit
 *       ('0'..'9'), else 0. n == 0 is vacuously true (returns 1) even
 *       if s is NULL. If n > 0 and s is NULL, returns 0.
 *   int __gj_ascii_is_digit_n  (alias)
 *   __libcgj_batch657_marker = "libcgj-batch657"
 *
 * Distinct from ctype isdigit / isdigit_l (single int code unit,
 * locale graph) and from NUL-terminated walks. Does not define isdigit
 * or related ctype surfaces — unique gj_ names only; avoid multi-def.
 * Digit test is ASCII range only (0x30..0x39).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch657_marker[] = "libcgj-batch657";

/* ---- freestanding helpers (not exported; not isdigit) ------------------ */

/* True if one unsigned octet is ASCII '0'..'9'. */
static int
b657_is_digit(unsigned char u8Ch)
{
	return (u8Ch >= (unsigned char)'0' && u8Ch <= (unsigned char)'9')
	    ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_is_digit_n — true if s[0..n) is all ASCII decimal digits.
 *
 * s: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 1, empty window is vacuously digits)
 *
 * Walks exactly n bytes (NUL is not a stop). Early-exits on the first
 * non-digit. Does not call libc isdigit.
 */
int
gj_ascii_is_digit_n(const char *s, size_t n)
{
	const unsigned char *pu;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (s == NULL) {
		return 0;
	}

	pu = (const unsigned char *)s;
	for (i = 0u; i < n; i++) {
		if (!b657_is_digit(pu[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_is_digit_n(const char *s, size_t n)
    __attribute__((alias("gj_ascii_is_digit_n")));
