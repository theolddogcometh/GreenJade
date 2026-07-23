/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8172: assert elide catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_assert_elide_id_8172(void);
 *     - Return 0: soft catalog id for fully elided assert body
 *       (matches ((void)0) status under NDEBUG).
 *   uint32_t __gj_assert_elide_id_8172  (alias)
 *   __libcgj_batch8172_marker = "libcgj-batch8172"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_assert_elide_id_8172 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8172_marker[] = "libcgj-batch8172";

/* Catalog: elided assert body is void-zero status. */
#define B8172_ASSERT_ELIDE_ID ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8172_assert_elide_id(void)
{
	return B8172_ASSERT_ELIDE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_assert_elide_id_8172 - soft elided-assert catalog id.
 *
 * Always returns 0. Mirrors the ((void)0) expansion of assert under
 * NDEBUG. Soft catalog only; never aborts. No parent wires.
 */
uint32_t
gj_assert_elide_id_8172(void)
{
	(void)NULL;
	return b8172_assert_elide_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_assert_elide_id_8172(void)
    __attribute__((alias("gj_assert_elide_id_8172")));
