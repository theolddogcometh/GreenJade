/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8176: NDEBUG debug-mode catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ndebug_debug_mode_8176(void);
 *     - Return 0: soft catalog that debug/assert-on mode is NOT
 *       selected when NDEBUG elision is the active policy.
 *   uint32_t __gj_ndebug_debug_mode_8176  (alias)
 *   __libcgj_batch8176_marker = "libcgj-batch8176"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_ndebug_debug_mode_8176 surface
 * only; no multi-def. Pair of gj_ndebug_release_mode_8175.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8176_marker[] = "libcgj-batch8176";

/* Catalog: debug/assert-on mode is off under NDEBUG policy. */
#define B8176_DEBUG_MODE ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8176_debug_mode(void)
{
	return B8176_DEBUG_MODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ndebug_debug_mode_8176 - soft debug-mode catalog id (NDEBUG off).
 *
 * Always returns 0. Pair of gj_ndebug_release_mode_8175 (returns 1).
 * Soft catalog only. No parent wires.
 */
uint32_t
gj_ndebug_debug_mode_8176(void)
{
	(void)NULL;
	return b8176_debug_mode();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ndebug_debug_mode_8176(void)
    __attribute__((alias("gj_ndebug_debug_mode_8176")));
