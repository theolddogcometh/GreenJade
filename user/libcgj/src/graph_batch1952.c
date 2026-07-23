/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1952: package-version character soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_pkg_ver_char_ok(int ch);
 *     — Soft validity hint: return 1 if ch is a single character allowed
 *       in a packaging version token (A-Za-z0-9 and one of
 *       '.', '+', '-', '~', ':'), else 0. Pure character-class data
 *       check; does not validate epoch/upstream/debian segment layout.
 *   int __gj_pkg_ver_char_ok  (alias)
 *   __libcgj_batch1952_marker = "libcgj-batch1952"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1952_marker[] = "libcgj-batch1952";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a soft-ok package-version character. */
static int
b1952_char_ok(int nCh)
{
	if (nCh >= 'A' && nCh <= 'Z') {
		return 1;
	}
	if (nCh >= 'a' && nCh <= 'z') {
		return 1;
	}
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	if (nCh == '.' || nCh == '+' || nCh == '-' ||
	    nCh == '~' || nCh == ':') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pkg_ver_char_ok — 1 iff ch is a soft-ok package-version character.
 *
 * ch: candidate character as int (EOF-style negative values rejected)
 * Returns 1 when in [A-Za-z0-9.+~:-], else 0.
 */
int
gj_pkg_ver_char_ok(int nCh)
{
	(void)NULL;
	return b1952_char_ok(nCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pkg_ver_char_ok(int nCh)
    __attribute__((alias("gj_pkg_ver_char_ok")));
