/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8567: glob path-separator predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_path_sep_u_8567(uint32_t ch);
 *     - Return 1 if ch is Unix path separator '/', else 0.
 *       Soft FNM_PATHNAME-related catalog check.
 *   uint32_t __gj_glob_path_sep_u_8567  (alias)
 *   __libcgj_batch8567_marker = "libcgj-batch8567"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_path_sep_u_8567 surface only; no multi-def.
 * Distinct from gj_path_sep_p_u (batch2827). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8567_marker[] = "libcgj-batch8567";

/* Unix path separator '/' (ASCII 0x2F). */
#define B8567_PATH_SEP  ((uint32_t)0x2Fu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8567_glob_path_sep(uint32_t u32Ch)
{
	return (u32Ch == B8567_PATH_SEP) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_path_sep_u_8567 - 1 if ch is '/'.
 *
 * ch: candidate code unit (unsigned view)
 *
 * Returns 1 for ASCII '/' (0x2F), else 0. Soft catalog predicate for
 * path-component boundaries under FNM_PATHNAME; does not walk paths.
 * No parent wires.
 */
uint32_t
gj_glob_path_sep_u_8567(uint32_t u32Ch)
{
	(void)NULL;
	return b8567_glob_path_sep(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_path_sep_u_8567(uint32_t u32Ch)
    __attribute__((alias("gj_glob_path_sep_u_8567")));
