/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1599: environment variable name character.
 *
 * Surface (unique symbols):
 *   int gj_env_is_name_char_u(int c);
 *     — Return 1 if c is valid in a POSIX-ish env name body:
 *       A-Z, a-z, 0-9, or underscore. Else 0.
 *   int __gj_env_is_name_char_u  (alias)
 *   __libcgj_batch1599_marker = "libcgj-batch1599"
 *
 * First character of a name is often restricted further (no digit) by
 * the caller; this predicate is per-octet body form.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1599_marker[] = "libcgj-batch1599";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1599_is_name_char(int nC)
{
	if (nC >= 'A' && nC <= 'Z') {
		return 1;
	}
	if (nC >= 'a' && nC <= 'z') {
		return 1;
	}
	if (nC >= '0' && nC <= '9') {
		return 1;
	}
	if (nC == '_') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_is_name_char_u — true for [A-Za-z0-9_].
 */
int
gj_env_is_name_char_u(int nC)
{
	return b1599_is_name_char(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_env_is_name_char_u(int nC)
    __attribute__((alias("gj_env_is_name_char_u")));
