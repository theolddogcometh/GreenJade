/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8178: NDEBUG assert void-cast stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_void_cast_u_8178(uint32_t cond);
 *     - Discard cond; always return 0. Soft model of ((void)(cond))
 *       / ((void)0) void-cast status under NDEBUG.
 *   uint32_t __gj_assert_void_cast_u_8178  (alias)
 *   __libcgj_batch8178_marker = "libcgj-batch8178"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_void_cast_u_8178
 * surface only; no multi-def. Distinct from
 * gj_assert_expr_ignore_u_8174 (same numeric form, separate surface).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8178_marker[] = "libcgj-batch8178";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8178_void_cast(uint32_t u32Cond)
{
	(void)u32Cond;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_void_cast_u_8178 - soft void-cast status of assert (NDEBUG).
 *
 * cond: unused condition under NDEBUG elision
 *
 * Always returns 0. Soft stub only. No parent wires.
 */
uint32_t
gj_assert_void_cast_u_8178(uint32_t u32Cond)
{
	(void)NULL;
	return b8178_void_cast(u32Cond);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_void_cast_u_8178(uint32_t u32Cond)
    __attribute__((alias("gj_assert_void_cast_u_8178")));
