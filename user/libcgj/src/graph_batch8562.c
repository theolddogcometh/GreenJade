/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8562: glob '?' metachar catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_qmark_id_8562(void);
 *     - Return soft ASCII code for glob question-mark metachar '?'
 *       (0x3F).
 *   uint32_t __gj_glob_qmark_id_8562  (alias)
 *   __libcgj_batch8562_marker = "libcgj-batch8562"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_qmark_id_8562 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8562_marker[] = "libcgj-batch8562";

/* Glob question-mark metacharacter '?' (ASCII 0x3F). */
#define B8562_GLOB_QMARK  ((uint32_t)0x3Fu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8562_glob_qmark_id(void)
{
	return B8562_GLOB_QMARK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_qmark_id_8562 - soft '?' glob metachar catalog id.
 *
 * Always returns 0x3F (ASCII '?'). Catalog id only; does not call
 * glob(3)/fnmatch(3). No parent wires.
 */
uint32_t
gj_glob_qmark_id_8562(void)
{
	(void)NULL;
	return b8562_glob_qmark_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_qmark_id_8562(void)
    __attribute__((alias("gj_glob_qmark_id_8562")));
