/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8177: NDEBUG assert-pass predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_pass_u_8177(uint32_t cond);
 *     - Discard cond; always return 1. Soft "assert always passes"
 *       under NDEBUG elision (never fails / never aborts).
 *   uint32_t __gj_assert_pass_u_8177  (alias)
 *   __libcgj_batch8177_marker = "libcgj-batch8177"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_pass_u_8177 surface
 * only; no multi-def. Distinct from gj_assert_noop_u_8173 (same
 * numeric form, separate call-site surface). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8177_marker[] = "libcgj-batch8177";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8177_assert_pass(uint32_t u32Cond)
{
	(void)u32Cond;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_pass_u_8177 - NDEBUG-style assert always-pass probe.
 *
 * cond: unused condition under NDEBUG elision
 *
 * Always returns 1. Soft stub only; never aborts. No parent wires.
 */
uint32_t
gj_assert_pass_u_8177(uint32_t u32Cond)
{
	(void)NULL;
	return b8177_assert_pass(u32Cond);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_pass_u_8177(uint32_t u32Cond)
    __attribute__((alias("gj_assert_pass_u_8177")));
