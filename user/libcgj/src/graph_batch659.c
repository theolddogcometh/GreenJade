/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch659: length-bounded ASCII alnum span predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_is_alnum_n(const char *s, size_t n);
 *     — Return 1 if every octet in s[0..n) is an ASCII alphanumeric
 *       character ('0'..'9', 'A'..'Z', or 'a'..'z'), else 0. n == 0 is
 *       vacuously true (returns 1) even if s is NULL. If n > 0 and s is
 *       NULL, returns 0.
 *   int __gj_ascii_is_alnum_n  (alias)
 *   __libcgj_batch659_marker = "libcgj-batch659"
 *
 * Distinct from ctype isalnum / isalnum_l (single int code unit, locale
 * graph), from gj_ascii_is_digit_n (batch657 digits only), and from
 * NUL-terminated walks. Does not define isalnum or related ctype
 * surfaces — unique gj_ names only; avoid multi-def. Class test is
 * ASCII range only (0x30..0x39, 0x41..0x5A, 0x61..0x7A).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch659_marker[] = "libcgj-batch659";

/* ---- freestanding helpers (not exported; not isalnum) ------------------ */

/* True if one unsigned octet is ASCII letter or decimal digit. */
static int
b659_is_alnum(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'0' && u8Ch <= (unsigned char)'9') {
		return 1;
	}
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return 1;
	}
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_is_alnum_n — true if s[0..n) is all ASCII alphanumeric octets.
 *
 * s: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 1, empty window is vacuously alnum)
 *
 * Walks exactly n bytes (NUL is not a stop). Early-exits on the first
 * non-alnum. Does not call libc isalnum.
 */
int
gj_ascii_is_alnum_n(const char *s, size_t n)
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
		if (!b659_is_alnum(pu[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_is_alnum_n(const char *s, size_t n)
    __attribute__((alias("gj_ascii_is_alnum_n")));
