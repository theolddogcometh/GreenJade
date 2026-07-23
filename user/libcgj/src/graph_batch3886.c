/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3886: INI key character predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ini_is_key_char_u(int c);
 *     - Return 1 if c is valid in an INI key body (ASCII subset):
 *       A-Z, a-z, 0-9, underscore, hyphen, or period. Else 0.
 *       Values outside 0..255 → 0.
 *   int __gj_ini_is_key_char_u  (alias)
 *   __libcgj_batch3886_marker = "libcgj-batch3886"
 *
 * Exclusive continuum CREATE-ONLY (3881-3890). Distinct from
 * gj_ini_is_keyval_u (batch3367 line form) — per-octet key body
 * predicate; unique name; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3886_marker[] = "libcgj-batch3886";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3886_is_key_char(int nC)
{
	if (nC < 0 || nC > 255) {
		return 0;
	}
	if (nC >= 'A' && nC <= 'Z') {
		return 1;
	}
	if (nC >= 'a' && nC <= 'z') {
		return 1;
	}
	if (nC >= '0' && nC <= '9') {
		return 1;
	}
	if (nC == '_' || nC == '-' || nC == '.') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ini_is_key_char_u — true for INI key body octets [A-Za-z0-9_.-].
 *
 * Does not allow '=', ';', '#', whitespace, or brackets (section marks).
 * First character may be further restricted by the caller. No parent wires.
 */
int
gj_ini_is_key_char_u(int nC)
{
	(void)NULL;
	return b3886_is_key_char(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_is_key_char_u(int nC)
    __attribute__((alias("gj_ini_is_key_char_u")));
