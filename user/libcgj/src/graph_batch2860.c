/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2860: milestone 2860 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2860(void);
 *     — Returns the compile-time graph batch number for this TU (2860).
 *   uint32_t __gj_batch_id_2860  (alias)
 *   __libcgj_batch2860_marker = "libcgj-batch2860"
 *
 * Milestone for the exclusive bitmap helpers wave (batches 2851-2859:
 * test_u, set_u, clr_u, flip_u, count_u, first_set_u, first_clr_u,
 * or_u, and_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2860_marker[] = "libcgj-batch2860";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2860_id(void)
{
	return 2860u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2860 — report this TU's graph batch number.
 *
 * Always returns 2860. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2860(void)
{
	(void)NULL;
	return b2860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2860(void)
    __attribute__((alias("gj_batch_id_2860")));
