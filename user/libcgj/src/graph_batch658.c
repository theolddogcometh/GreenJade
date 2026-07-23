/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch658: length-bounded ASCII alphabetic span
 * predicate (A-Za-z only).
 *
 * Surface (unique symbols):
 *   int gj_ascii_is_alpha_n(const char *s, size_t n);
 *     — Return 1 (OK) if every octet in s[0..n) is an ASCII letter
 *       ('A'..'Z' or 'a'..'z'), else 0 (FAIL). n == 0 is vacuously true
 *       (returns 1) even if s is NULL. If n > 0 and s is NULL, returns 0.
 *   int __gj_ascii_is_alpha_n  (alias)
 *   __libcgj_batch658_marker = "libcgj-batch658"
 *
 * Distinct from ctype isalpha / isalpha_l (single int code unit,
 * locale graph), from gj_ascii_is_digit_n (batch657), and from
 * NUL-terminated walks. Does not define isalpha or related ctype
 * surfaces — unique gj_ names only; avoid multi-def. Letter test is
 * ASCII range only (0x41..0x5A / 0x61..0x7A); no locale, no underscore.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch658_marker[] = "libcgj-batch658";

/* ---- freestanding helpers (not exported; not isalpha) ------------------ */

/* True if one unsigned octet is ASCII 'A'..'Z' or 'a'..'z'. */
static int
b658_is_alpha(unsigned char u8Ch)
{
	return ((u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') ||
	        (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z'))
	    ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_is_alpha_n — true (OK) if s[0..n) is all ASCII letters.
 *
 * s: region of at least n bytes when n > 0 (NULL with n > 0 → 0 / FAIL)
 * n: number of octets to scan (0 → 1 / OK, empty window is vacuously alpha)
 *
 * Walks exactly n bytes (NUL is not a stop). Early-exits on the first
 * non-letter. Does not call libc isalpha. A-Za-z only.
 */
int
gj_ascii_is_alpha_n(const char *s, size_t n)
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
		if (!b658_is_alpha(pu[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_is_alpha_n(const char *s, size_t n)
    __attribute__((alias("gj_ascii_is_alpha_n")));
