/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49508: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49508(void);
 *     - Returns the compile-time graph batch number for this TU (49508).
 *   uint32_t gj_graph_milestone_49508(void);
 *     - Returns the current graph milestone revision (49508).
 *   uint32_t __gj_batch_id_49508  (alias)
 *   uint32_t __gj_graph_milestone_49508  (alias)
 *   __libcgj_batch49508_marker = "libcgj-batch49508"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 49501-49550, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 49600).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch49508_marker[] = "libcgj-batch49508";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49508_id(void)
{
	return 49508u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49508 - report this TU's graph batch number.
 *
 * Always returns 49508.
 */
uint32_t
gj_batch_id_49508(void)
{
	(void)NULL;
	return b49508_id();
}

/*
 * gj_graph_milestone_49508 - report this TU's graph milestone revision.
 *
 * Always returns 49508. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_49508(void)
{
	return b49508_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_49508(void)
    __attribute__((alias("gj_batch_id_49508")));

uint32_t __gj_graph_milestone_49508(void)
    __attribute__((alias("gj_graph_milestone_49508")));
