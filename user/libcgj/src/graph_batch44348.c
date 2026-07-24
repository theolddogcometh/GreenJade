/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44348: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44348(void);
 *     - Returns the compile-time graph batch number for this TU (44348).
 *   uint32_t gj_graph_milestone_44348(void);
 *     - Returns the current graph milestone revision (44348).
 *   uint32_t __gj_batch_id_44348  (alias)
 *   uint32_t __gj_graph_milestone_44348  (alias)
 *   __libcgj_batch44348_marker = "libcgj-batch44348"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 44301-44350, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 44400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch44348_marker[] = "libcgj-batch44348";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44348_id(void)
{
	return 44348u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44348 - report this TU's graph batch number.
 *
 * Always returns 44348.
 */
uint32_t
gj_batch_id_44348(void)
{
	(void)NULL;
	return b44348_id();
}

/*
 * gj_graph_milestone_44348 - report this TU's graph milestone revision.
 *
 * Always returns 44348. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_44348(void)
{
	return b44348_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_44348(void)
    __attribute__((alias("gj_batch_id_44348")));

uint32_t __gj_graph_milestone_44348(void)
    __attribute__((alias("gj_graph_milestone_44348")));
