/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8335: Unix path-separator char probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_sep_u_8335(uint32_t ch);
 *     - Return 1 if ch is the Unix directory separator '/'; else 0.
 *       Only the exact byte 0x2F matches. Does not treat '\\' as a sep.
 *   int __gj_path_sep_u_8335  (alias)
 *   __libcgj_batch8335_marker = "libcgj-batch8335"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from gj_path_sep_p_u
 * (batch2827) and gj_path_is_sep_u (batch2471 dual '/'|'\\') — unique
 * _u_8335 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8335_marker[] = "libcgj-batch8335";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is the Unix path separator '/'. */
static int
b8335_sep(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_sep_u_8335 - 1 if ch is '/', else 0.
 *
 * ch: character code unit
 *
 * Returns 1 on Unix separator match, 0 otherwise (including '\\').
 */
int
gj_path_sep_u_8335(uint32_t u32Ch)
{
	(void)NULL;
	return b8335_sep(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_sep_u_8335(uint32_t u32Ch)
    __attribute__((alias("gj_path_sep_u_8335")));
