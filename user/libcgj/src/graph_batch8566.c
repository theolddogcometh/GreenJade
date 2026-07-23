/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8566: glob literal code-unit match stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glob_match_lit_u_8566(uint32_t a, uint32_t b);
 *     - Return 1 if a == b (literal code-unit equality), else 0.
 *   uint32_t __gj_glob_match_lit_u_8566  (alias)
 *   __libcgj_batch8566_marker = "libcgj-batch8566"
 *
 * Exclusive continuum CREATE-ONLY (8561-8570: glob pattern stubs —
 * star_id, qmark_id, bracket_ok_u, escape_ok_u, match_empty_u,
 * match_lit_u, path_sep_u, ready_u, flags_ok_u, batch_id_8570).
 * Unique gj_glob_match_lit_u_8566 surface only; no multi-def.
 * Distinct from gj_glob_match (batch150). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8566_marker[] = "libcgj-batch8566";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8566_glob_match_lit(uint32_t u32A, uint32_t u32B)
{
	return (u32A == u32B) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glob_match_lit_u_8566 - 1 if code units a and b are equal.
 *
 * a: pattern-side code unit (unsigned view)
 * b: subject-side code unit (unsigned view)
 *
 * Returns 1 when a == b, else 0. Soft single-unit literal compare; no
 * casefold, no multi-byte decode, no glob(3). No parent wires.
 */
uint32_t
gj_glob_match_lit_u_8566(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8566_glob_match_lit(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glob_match_lit_u_8566(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_glob_match_lit_u_8566")));
