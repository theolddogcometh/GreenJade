/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36205: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36205(void);
 *     - Returns the compile-time graph batch number for this TU (36205).
 *   uint32_t gj_graph_milestone_36205(void);
 *     - Returns the current graph milestone revision (36205).
 *   uint32_t __gj_batch_id_36205  (alias)
 *   uint32_t __gj_graph_milestone_36205  (alias)
 *   __libcgj_batch36205_marker = "libcgj-batch36205"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 36201-36250, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 36300).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch36205_marker[] = "libcgj-batch36205";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36205_id(void)
{
	return 36205u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36205 - report this TU's graph batch number.
 *
 * Always returns 36205.
 */
uint32_t
gj_batch_id_36205(void)
{
	(void)NULL;
	return b36205_id();
}

/*
 * gj_graph_milestone_36205 - report this TU's graph milestone revision.
 *
 * Always returns 36205. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_36205(void)
{
	return b36205_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36205(void)
    __attribute__((alias("gj_batch_id_36205")));

uint32_t __gj_graph_milestone_36205(void)
    __attribute__((alias("gj_graph_milestone_36205")));
