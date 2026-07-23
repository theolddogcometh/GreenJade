/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8564: glob escape metachar predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_escape_ok_u_8564(uint32_t ch);
 *     - Return 1 if ch is the backslash escape introducer '\\',
 *       else 0.
 *   uint32_t __gj_glob_escape_ok_u_8564  (alias)
 *   __libcgj_batch8564_marker = "libcgj-batch8564"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_escape_ok_u_8564 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8564_marker[] = "libcgj-batch8564";

/* Backslash escape introducer '\\' (ASCII 0x5C). */
#define B8564_ESCAPE  ((uint32_t)0x5Cu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8564_glob_escape_ok(uint32_t u32Ch)
{
	return (u32Ch == B8564_ESCAPE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_escape_ok_u_8564 - 1 if ch is '\\'.
 *
 * ch: candidate code unit (unsigned view)
 *
 * Returns 1 for ASCII backslash (0x5C), else 0. Soft catalog
 * predicate for FNM_NOESCAPE-related checks; does not parse escapes.
 * No parent wires.
 */
uint32_t
gj_glob_escape_ok_u_8564(uint32_t u32Ch)
{
	(void)NULL;
	return b8564_glob_escape_ok(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_escape_ok_u_8564(uint32_t u32Ch)
    __attribute__((alias("gj_glob_escape_ok_u_8564")));
