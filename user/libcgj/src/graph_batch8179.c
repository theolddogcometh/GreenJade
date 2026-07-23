/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8179: NDEBUG assert condition-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_cond_ok_u_8179(uint32_t cond);
 *     - Soft NDEBUG hold: always return 1 (condition treated as ok
 *       because the assert is elided). cond is unused.
 *   uint32_t __gj_assert_cond_ok_u_8179  (alias)
 *   __libcgj_batch8179_marker = "libcgj-batch8179"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_cond_ok_u_8179 surface
 * only; no multi-def. Distinct from gj_dbg_assert_ok_u (batch5085,
 * which booleanizes cond). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8179_marker[] = "libcgj-batch8179";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8179_cond_ok(uint32_t u32Cond)
{
	(void)u32Cond;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_cond_ok_u_8179 - NDEBUG soft condition-ok (always 1).
 *
 * cond: unused condition under NDEBUG elision
 *
 * Always returns 1. Does not booleanize cond (unlike soft debug
 * probes that test nonzero). No parent wires.
 */
uint32_t
gj_assert_cond_ok_u_8179(uint32_t u32Cond)
{
	(void)NULL;
	return b8179_cond_ok(u32Cond);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_cond_ok_u_8179(uint32_t u32Cond)
    __attribute__((alias("gj_assert_cond_ok_u_8179")));
