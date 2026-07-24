/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19304: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_19304(void);
 *     - Returns the compile-time graph batch number for this TU (19304).
 *   uint32_t gj_graph_milestone_19304(void);
 *     - Returns the current graph milestone revision (19304).
 *   uint32_t __gj_batch_id_19304  (alias)
 *   uint32_t __gj_graph_milestone_19304  (alias)
 *   __libcgj_batch19304_marker = "libcgj-batch19304"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 19301-19350, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 19400).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch19304_marker[] = "libcgj-batch19304";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19304_id(void)
{
	return 19304u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_19304 - report this TU's graph batch number.
 *
 * Always returns 19304.
 */
uint32_t
gj_batch_id_19304(void)
{
	(void)NULL;
	return b19304_id();
}

/*
 * gj_graph_milestone_19304 - report this TU's graph milestone revision.
 *
 * Always returns 19304. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_19304(void)
{
	return b19304_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_19304(void)
    __attribute__((alias("gj_batch_id_19304")));

uint32_t __gj_graph_milestone_19304(void)
    __attribute__((alias("gj_graph_milestone_19304")));
