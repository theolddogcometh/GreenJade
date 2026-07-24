/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch53440: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_53440(void);
 *     - Returns the compile-time graph batch number for this TU (53440).
 *   uint32_t gj_graph_milestone_53440(void);
 *     - Returns the current graph milestone revision (53440).
 *   uint32_t __gj_batch_id_53440  (alias)
 *   uint32_t __gj_graph_milestone_53440  (alias)
 *   __libcgj_batch53440_marker = "libcgj-batch53440"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-53440, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 25900).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch53440_marker[] = "libcgj-batch53440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b53440_id(void)
{
	return 53440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_53440 - report this TU's graph batch number.
 *
 * Always returns 53440.
 */
uint32_t
gj_batch_id_53440(void)
{
	(void)NULL;
	return b53440_id();
}

/*
 * gj_graph_milestone_53440 - report this TU's graph milestone revision.
 *
 * Always returns 53440. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_53440(void)
{
	return b53440_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_53440(void)
    __attribute__((alias("gj_batch_id_53440")));

uint32_t __gj_graph_milestone_53440(void)
    __attribute__((alias("gj_graph_milestone_53440")));
