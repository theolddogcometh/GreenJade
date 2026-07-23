/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8561: glob '*' metachar catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_star_id_8561(void);
 *     - Return soft ASCII code for glob star metachar '*' (0x2A).
 *   uint32_t __gj_glob_star_id_8561  (alias)
 *   __libcgj_batch8561_marker = "libcgj-batch8561"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_star_id_8561 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8561_marker[] = "libcgj-batch8561";

/* Glob star metacharacter '*' (ASCII 0x2A). */
#define B8561_GLOB_STAR  ((uint32_t)0x2Au)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8561_glob_star_id(void)
{
	return B8561_GLOB_STAR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_star_id_8561 - soft '*' glob metachar catalog id.
 *
 * Always returns 0x2A (ASCII '*'). Catalog id only; does not call
 * glob(3)/fnmatch(3). No parent wires.
 */
uint32_t
gj_glob_star_id_8561(void)
{
	(void)NULL;
	return b8561_glob_star_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_star_id_8561(void)
    __attribute__((alias("gj_glob_star_id_8561")));
