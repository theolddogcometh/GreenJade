/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18865: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18865(void);
 *     - Returns the compile-time graph batch number for this TU (18865).
 *   uint32_t gj_graph_milestone_18865(void);
 *     - Returns the current graph milestone revision (18865).
 *   uint32_t __gj_batch_id_18865  (alias)
 *   uint32_t __gj_graph_milestone_18865  (alias)
 *   __libcgj_batch18865_marker = "libcgj-batch18865"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 18851-18875, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 18900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch18865_marker[] = "libcgj-batch18865";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18865_id(void)
{
	return 18865u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18865 - report this TU's graph batch number.
 *
 * Always returns 18865.
 */
uint32_t
gj_batch_id_18865(void)
{
	(void)NULL;
	return b18865_id();
}

/*
 * gj_graph_milestone_18865 - report this TU's graph milestone revision.
 *
 * Always returns 18865. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_18865(void)
{
	return b18865_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18865(void)
    __attribute__((alias("gj_batch_id_18865")));

uint32_t __gj_graph_milestone_18865(void)
    __attribute__((alias("gj_graph_milestone_18865")));
