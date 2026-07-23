/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch914: INI comment introducer predicate.
 *
 * Surface (unique symbols):
 *   int gj_ini_is_comment_char(int c);
 *     — Return 1 if c starts an INI line comment (';' or '#'), else 0.
 *   int __gj_ini_is_comment_char  (alias)
 *   __libcgj_batch914_marker = "libcgj-batch914"
 *
 * Does NOT redefine gj_ini_get / gj_ini_get_int (batch104) — avoid
 * multi-def. Both classic (;) and hash-style (#) comment markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch914_marker[] = "libcgj-batch914";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ini_is_comment_char — true for ';' or '#'.
 *
 * Callers typically test the first non-whitespace character of a line.
 */
int
gj_ini_is_comment_char(int c)
{
	return (c == ';' || c == '#') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_is_comment_char(int c)
    __attribute__((alias("gj_ini_is_comment_char")));
