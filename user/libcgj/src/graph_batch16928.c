/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16928: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16928(void);
 *     - Returns the compile-time graph batch number for this TU (16928).
 *   uint32_t gj_graph_milestone_16928(void);
 *     - Returns the current graph milestone revision (16928).
 *   uint32_t __gj_batch_id_16928  (alias)
 *   uint32_t __gj_graph_milestone_16928  (alias)
 *   __libcgj_batch16928_marker = "libcgj-batch16928"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 16901-16950, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 17000).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch16928_marker[] = "libcgj-batch16928";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16928_id(void)
{
	return 16928u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16928 - report this TU's graph batch number.
 *
 * Always returns 16928.
 */
uint32_t
gj_batch_id_16928(void)
{
	(void)NULL;
	return b16928_id();
}

/*
 * gj_graph_milestone_16928 - report this TU's graph milestone revision.
 *
 * Always returns 16928. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_16928(void)
{
	return b16928_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_16928(void)
    __attribute__((alias("gj_batch_id_16928")));

uint32_t __gj_graph_milestone_16928(void)
    __attribute__((alias("gj_graph_milestone_16928")));
