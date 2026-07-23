/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch661: length-bounded ASCII printable predicate
 * (space through tilde, 0x20..0x7E inclusive).
 *
 * Surface (unique symbols):
 *   int gj_ascii_is_print_n(const char *s, size_t n);
 *     — Return 1 (OK) if every octet in s[0..n) is in the closed range
 *       0x20..0x7E (ASCII space through '~'). Else return 0 (FAIL).
 *       n == 0 is vacuously OK (returns 1) even if s is NULL. If n > 0
 *       and s is NULL, returns 0 (FAIL). Does not stop at NUL; walks
 *       exactly n bytes. Does not treat DEL (0x7F), control octets, or
 *       high-bit / locale isprint classes as printable.
 *   int __gj_ascii_is_print_n  (alias)
 *   __libcgj_batch661_marker = "libcgj-batch661"
 *
 * Distinct from ctype isprint / isprint_l (locale-aware, single code
 * unit) and from gj_ascii_is_space_n / gj_ascii_is_alnum_n (narrower
 * classes). Unique gj_ names only; no multi-def with the existing libc
 * graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc helpers (no isprint / strlen). No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch661_marker[] = "libcgj-batch661";

/* ---- freestanding helpers (not exported) ------------------------------- */

/* True if ch is ASCII printable: 0x20 (space) through 0x7E ('~'). */
static int
b661_is_print(unsigned char u8Ch)
{
	return u8Ch >= 0x20u && u8Ch <= 0x7Eu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_is_print_n — true (OK) if s[0..n) is only ASCII printables.
 *
 * s: region of at least n bytes when n > 0 (NULL with n > 0 → 0 / FAIL)
 * n: number of octets to scan (0 → 1 / OK, empty window is vacuously
 *    all-printable)
 *
 * Does not call libc. Early-exits on the first non-printable octet.
 * Returns 1 on success (OK), 0 otherwise (FAIL).
 */
int
gj_ascii_is_print_n(const char *s, size_t n)
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
		if (!b661_is_print(pu[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_is_print_n(const char *s, size_t n)
    __attribute__((alias("gj_ascii_is_print_n")));
