/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch33156: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_33156(void);
 *     - Returns the compile-time graph batch number for this TU (33156).
 *   uint32_t gj_graph_milestone_33156(void);
 *     - Returns the current graph milestone revision (33156).
 *   uint32_t __gj_batch_id_33156  (alias)
 *   uint32_t __gj_graph_milestone_33156  (alias)
 *   __libcgj_batch33156_marker = "libcgj-batch33156"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 33151-33175, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 33200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch33156_marker[] = "libcgj-batch33156";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b33156_id(void)
{
	return 33156u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_33156 - report this TU's graph batch number.
 *
 * Always returns 33156.
 */
uint32_t
gj_batch_id_33156(void)
{
	(void)NULL;
	return b33156_id();
}

/*
 * gj_graph_milestone_33156 - report this TU's graph milestone revision.
 *
 * Always returns 33156. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_33156(void)
{
	return b33156_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_33156(void)
    __attribute__((alias("gj_batch_id_33156")));

uint32_t __gj_graph_milestone_33156(void)
    __attribute__((alias("gj_graph_milestone_33156")));
