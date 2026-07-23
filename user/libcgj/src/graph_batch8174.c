/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8174: NDEBUG assert expression-ignore stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_expr_ignore_u_8174(uint32_t expr);
 *     - Discard expr (not evaluated under true NDEBUG); return 0 as
 *       void-status of ((void)0).
 *   uint32_t __gj_assert_expr_ignore_u_8174  (alias)
 *   __libcgj_batch8174_marker = "libcgj-batch8174"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_expr_ignore_u_8174
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8174_marker[] = "libcgj-batch8174";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8174_expr_ignore(uint32_t u32Expr)
{
	(void)u32Expr;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_expr_ignore_u_8174 - ignore assert expression (NDEBUG).
 *
 * expr: unused expression value (elided under NDEBUG)
 *
 * Always returns 0 (void-status). Soft stub only; does not evaluate
 * side effects or abort. No parent wires.
 */
uint32_t
gj_assert_expr_ignore_u_8174(uint32_t u32Expr)
{
	(void)NULL;
	return b8174_expr_ignore(u32Expr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_expr_ignore_u_8174(uint32_t u32Expr)
    __attribute__((alias("gj_assert_expr_ignore_u_8174")));
