/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2471: freestanding path-separator char probe.
 *
 * Surface (unique symbols):
 *   unsigned gj_path_is_sep_u(int c);
 *     — Return 1u if c is a directory separator ('/' or '\\'), else 0u.
 *       Accepts the full int character range used by ctype-style APIs;
 *       only the exact bytes 0x2F and 0x5C match.
 *   unsigned __gj_path_is_sep_u  (alias)
 *   __libcgj_batch2471_marker = "libcgj-batch2471"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_has_sep
 * (batch683 whole-string scan) — this is a single-character predicate.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2471_marker[] = "libcgj-batch2471";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b2471_is_sep(int nC)
{
	if (nC == '/' || nC == '\\') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_sep_u — 1u if c is '/' or '\\', else 0u.
 *
 * c: character code (promoted int, ctype style)
 * Returns 1u on separator match, 0u otherwise.
 */
unsigned
gj_path_is_sep_u(int nC)
{
	(void)NULL;
	return b2471_is_sep(nC);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_path_is_sep_u(int nC)
    __attribute__((alias("gj_path_is_sep_u")));
