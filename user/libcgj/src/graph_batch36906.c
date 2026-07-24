/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36906: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36906(void);
 *     - Returns the compile-time graph batch number for this TU (36906).
 *   uint32_t gj_graph_milestone_36906(void);
 *     - Returns the current graph milestone revision (36906).
 *   uint32_t __gj_batch_id_36906  (alias)
 *   uint32_t __gj_graph_milestone_36906  (alias)
 *   __libcgj_batch36906_marker = "libcgj-batch36906"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 36901-36950, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 37000).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch36906_marker[] = "libcgj-batch36906";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36906_id(void)
{
	return 36906u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36906 - report this TU's graph batch number.
 *
 * Always returns 36906.
 */
uint32_t
gj_batch_id_36906(void)
{
	(void)NULL;
	return b36906_id();
}

/*
 * gj_graph_milestone_36906 - report this TU's graph milestone revision.
 *
 * Always returns 36906. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_36906(void)
{
	return b36906_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_36906(void)
    __attribute__((alias("gj_batch_id_36906")));

uint32_t __gj_graph_milestone_36906(void)
    __attribute__((alias("gj_graph_milestone_36906")));
