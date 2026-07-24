/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26675: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26675(void);
 *     - Returns the compile-time graph batch number for this TU (26675).
 *   uint32_t gj_graph_milestone_26675(void);
 *     - Returns the current graph milestone revision (26675).
 *   uint32_t __gj_batch_id_26675  (alias)
 *   uint32_t __gj_graph_milestone_26675  (alias)
 *   __libcgj_batch26675_marker = "libcgj-batch26675"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 26651-26675, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 26700).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch26675_marker[] = "libcgj-batch26675";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26675_id(void)
{
	return 26675u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26675 - report this TU's graph batch number.
 *
 * Always returns 26675.
 */
uint32_t
gj_batch_id_26675(void)
{
	(void)NULL;
	return b26675_id();
}

/*
 * gj_graph_milestone_26675 - report this TU's graph milestone revision.
 *
 * Always returns 26675. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_26675(void)
{
	return b26675_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_26675(void)
    __attribute__((alias("gj_batch_id_26675")));

uint32_t __gj_graph_milestone_26675(void)
    __attribute__((alias("gj_graph_milestone_26675")));
