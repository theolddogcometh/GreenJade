/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8175: NDEBUG release-mode catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ndebug_release_mode_8175(void);
 *     - Return 1: soft catalog that release/NDEBUG mode is selected
 *       (asserts off).
 *   uint32_t __gj_ndebug_release_mode_8175  (alias)
 *   __libcgj_batch8175_marker = "libcgj-batch8175"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_ndebug_release_mode_8175
 * surface only; no multi-def. Pair of gj_ndebug_debug_mode_8176.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8175_marker[] = "libcgj-batch8175";

/* Catalog: release / NDEBUG mode is selected. */
#define B8175_RELEASE_MODE ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8175_release_mode(void)
{
	return B8175_RELEASE_MODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ndebug_release_mode_8175 - soft release/NDEBUG mode catalog id.
 *
 * Always returns 1. Pair of gj_ndebug_debug_mode_8176 (returns 0).
 * Soft catalog only. No parent wires.
 */
uint32_t
gj_ndebug_release_mode_8175(void)
{
	(void)NULL;
	return b8175_release_mode();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ndebug_release_mode_8175(void)
    __attribute__((alias("gj_ndebug_release_mode_8175")));
