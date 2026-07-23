/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8173: NDEBUG assert no-op predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_noop_u_8173(uint32_t cond);
 *     - Discard cond; always return 1. Soft NDEBUG assert no-op that
 *       reports "passed" without evaluating or aborting.
 *   uint32_t __gj_assert_noop_u_8173  (alias)
 *   __libcgj_batch8173_marker = "libcgj-batch8173"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_noop_u_8173 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8173_marker[] = "libcgj-batch8173";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8173_assert_noop(uint32_t u32Cond)
{
	(void)u32Cond;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_noop_u_8173 - NDEBUG-style assert no-op (always "ok").
 *
 * cond: unused condition (elided under NDEBUG semantics)
 *
 * Always returns 1. Soft stub only; never aborts or sets errno.
 * No parent wires.
 */
uint32_t
gj_assert_noop_u_8173(uint32_t u32Cond)
{
	(void)NULL;
	return b8173_assert_noop(u32Cond);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_noop_u_8173(uint32_t u32Cond)
    __attribute__((alias("gj_assert_noop_u_8173")));
