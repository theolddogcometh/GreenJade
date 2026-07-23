/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8180: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8180(void);
 *     - Returns the compile-time graph batch number for this TU (8180).
 *   uint32_t __gj_batch_id_8180  (alias)
 *   __libcgj_batch8180_marker = "libcgj-batch8180"
 *
 * Exclusive continuum CREATE-ONLY (8171-8180: assert NDEBUG stubs —
 * ndebug_active_id, assert_elide_id, assert_noop_u, expr_ignore_u,
 * release_mode_id, debug_mode_id, assert_pass_u, void_cast_u,
 * cond_ok_u, batch_id_8180). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8180_marker[] = "libcgj-batch8180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8180_id(void)
{
	return 8180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8180 - report this TU's graph batch number.
 *
 * Always returns 8180. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8180(void)
{
	(void)NULL;
	return b8180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8180(void)
    __attribute__((alias("gj_batch_id_8180")));
