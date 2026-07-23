/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8563: glob bracket metachar predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_bracket_ok_u_8563(uint32_t ch);
 *     - Return 1 if ch is a bracket class delimiter '[' or ']',
 *       else 0.
 *   uint32_t __gj_glob_bracket_ok_u_8563  (alias)
 *   __libcgj_batch8563_marker = "libcgj-batch8563"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_bracket_ok_u_8563 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8563_marker[] = "libcgj-batch8563";

/* Bracket class open/close metacharacters. */
#define B8563_LBRK  ((uint32_t)0x5Bu) /* '[' */
#define B8563_RBRK  ((uint32_t)0x5Du) /* ']' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8563_glob_bracket_ok(uint32_t u32Ch)
{
	if (u32Ch == B8563_LBRK || u32Ch == B8563_RBRK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_bracket_ok_u_8563 - 1 if ch is '[' or ']'.
 *
 * ch: candidate code unit (unsigned view)
 *
 * Returns 1 for ASCII '[' (0x5B) or ']' (0x5D), else 0. Soft catalog
 * predicate; does not parse character classes. No parent wires.
 */
uint32_t
gj_glob_bracket_ok_u_8563(uint32_t u32Ch)
{
	(void)NULL;
	return b8563_glob_bracket_ok(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_bracket_ok_u_8563(uint32_t u32Ch)
    __attribute__((alias("gj_glob_bracket_ok_u_8563")));
