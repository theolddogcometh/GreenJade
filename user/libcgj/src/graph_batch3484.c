/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3484: freestanding path-separator char probe.
 *
 * Surface (unique symbols):
 *   unsigned gj_path_is_sep_u2(int c);
 *     - Return 1u if c is a directory separator ('/' or '\\'), else 0u.
 *       Accepts the full int character range used by ctype-style APIs;
 *       only the exact bytes 0x2F and 0x5C match.
 *       Renamed from gj_path_is_sep_u (collision with batch2471).
 *   unsigned __gj_path_is_sep_u2  (alias)
 *   __libcgj_batch3484_marker = "libcgj-batch3484"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Distinct from
 * gj_path_is_sep_u (batch2471) and gj_path_sep_p_u (batch2827 Unix-only).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3484_marker[] = "libcgj-batch3484";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b3484_is_sep(int nC)
{
	if (nC == '/' || nC == '\\') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_sep_u2 - 1u if c is '/' or '\\', else 0u.
 *
 * c: character code (promoted int, ctype style)
 *
 * Renamed from planned gj_path_is_sep_u (batch2471 owns that symbol).
 * Does not call libc. No parent wires.
 */
unsigned
gj_path_is_sep_u2(int nC)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3484_is_sep(nC);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_path_is_sep_u2(int nC)
    __attribute__((alias("gj_path_is_sep_u2")));
