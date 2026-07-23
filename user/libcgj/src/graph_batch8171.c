/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8171: NDEBUG active catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ndebug_active_id_8171(void);
 *     - Return 1: soft catalog that NDEBUG-style assert elision is
 *       active (release/assert-off mode).
 *   uint32_t __gj_ndebug_active_id_8171  (alias)
 *   __libcgj_batch8171_marker = "libcgj-batch8171"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Unique gj_ndebug_active_id_8171 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8171_marker[] = "libcgj-batch8171";

/* Catalog: NDEBUG assert-elision mode is active. */
#define B8171_NDEBUG_ACTIVE ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8171_ndebug_active_id(void)
{
	return B8171_NDEBUG_ACTIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ndebug_active_id_8171 - soft NDEBUG-active catalog id.
 *
 * Always returns 1. Documents that assert macros behave as ((void)0)
 * under NDEBUG-style elision. Soft catalog only; does not define
 * NDEBUG or call abort(3). No parent wires.
 */
uint32_t
gj_ndebug_active_id_8171(void)
{
	(void)NULL;
	return b8171_ndebug_active_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ndebug_active_id_8171(void)
    __attribute__((alias("gj_ndebug_active_id_8171")));
