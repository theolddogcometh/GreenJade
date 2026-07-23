/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch916: environment variable name character.
 *
 * Surface (unique symbols):
 *   int gj_env_is_name_char(int c);
 *     — Return 1 if c is valid in a POSIX-ish env name body:
 *       A-Z, a-z, 0-9, or underscore. Else 0.
 *   int __gj_env_is_name_char  (alias)
 *   __libcgj_batch916_marker = "libcgj-batch916"
 *
 * Does NOT redefine gj_env_parse_line / gj_env_get (batch82) — avoid
 * multi-def. First character of a name is often restricted further
 * (no digit) by the caller; this predicate is per-octet body form.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch916_marker[] = "libcgj-batch916";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_is_name_char — true for [A-Za-z0-9_].
 */
int
gj_env_is_name_char(int c)
{
	if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	if (c >= '0' && c <= '9') {
		return 1;
	}
	if (c == '_') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_env_is_name_char(int c)
    __attribute__((alias("gj_env_is_name_char")));
