/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2827: Unix path-separator character predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_sep_p_u(int c);
 *     - Return 1 if c is the Unix directory separator '/'; else 0.
 *       Accepts the full int character range used by ctype-style APIs;
 *       only the exact byte 0x2F matches. Does not treat '\\' as a sep.
 *   int __gj_path_sep_p_u  (alias)
 *   __libcgj_batch2827_marker = "libcgj-batch2827"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_path_is_sep_u
 * (batch2471 dual '/'|'\\') — Unix-only separator predicate; unique
 * sep_p_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2827_marker[] = "libcgj-batch2827";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff nC is the Unix path separator '/'. */
static int
b2827_sep_p(int nC)
{
	if (nC == '/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_sep_p_u - 1 if c is '/', else 0.
 *
 * c: character code (promoted int, ctype style)
 *
 * Returns 1 on Unix separator match, 0 otherwise (including '\\').
 */
int
gj_path_sep_p_u(int nC)
{
	(void)NULL;
	return b2827_sep_p(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_sep_p_u(int nC)
    __attribute__((alias("gj_path_sep_p_u")));
