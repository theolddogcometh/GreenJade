/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8565: glob empty-length match predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_match_empty_u_8565(uint32_t len);
 *     - Return 1 if len is 0 (empty pattern/string length), else 0.
 *       Soft empty-match gate: empty length is the empty-string case.
 *   uint32_t __gj_glob_match_empty_u_8565  (alias)
 *   __libcgj_batch8565_marker = "libcgj-batch8565"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_match_empty_u_8565 surface only; no multi-def.
 * Distinct from gj_glob_match (batch150). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8565_marker[] = "libcgj-batch8565";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8565_glob_match_empty(uint32_t u32Len)
{
	return (u32Len == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_match_empty_u_8565 - 1 if length is empty (0).
 *
 * len: soft pattern or subject length in code units
 *
 * Returns 1 when len == 0 (empty-string match case), else 0. Integer
 * catalog test only; does not walk strings or call glob(3). No parent
 * wires.
 */
uint32_t
gj_glob_match_empty_u_8565(uint32_t u32Len)
{
	(void)NULL;
	return b8565_glob_match_empty(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_match_empty_u_8565(uint32_t u32Len)
    __attribute__((alias("gj_glob_match_empty_u_8565")));
