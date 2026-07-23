/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1951: package-name character soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_pkg_name_char_ok(int ch);
 *     — Soft validity hint: return 1 if ch is a single character allowed
 *       in a packaging product-path name token (lowercase a-z, digit
 *       0-9, or one of '+', '-', '.'), else 0. Pure character-class
 *       data check; does not enforce leading-alnum or whole-name rules.
 *   int __gj_pkg_name_char_ok  (alias)
 *   __libcgj_batch1951_marker = "libcgj-batch1951"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1951_marker[] = "libcgj-batch1951";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a soft-ok package-name character. */
static int
b1951_char_ok(int nCh)
{
	if (nCh >= 'a' && nCh <= 'z') {
		return 1;
	}
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	if (nCh == '+' || nCh == '-' || nCh == '.') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pkg_name_char_ok — 1 iff ch is a soft-ok package-name character.
 *
 * ch: candidate character as int (EOF-style negative values rejected)
 * Returns 1 when in [a-z0-9+.-], else 0.
 */
int
gj_pkg_name_char_ok(int nCh)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1951_char_ok(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pkg_name_char_ok(int nCh)
    __attribute__((alias("gj_pkg_name_char_ok")));
