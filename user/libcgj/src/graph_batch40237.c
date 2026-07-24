/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40237: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40237(void);
 *     - Returns the compile-time graph batch number for this TU (40237).
 *   uint32_t gj_graph_milestone_40237(void);
 *     - Returns the current graph milestone revision (40237).
 *   uint32_t __gj_batch_id_40237  (alias)
 *   uint32_t __gj_graph_milestone_40237  (alias)
 *   __libcgj_batch40237_marker = "libcgj-batch40237"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 40201-40250, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 40300).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch40237_marker[] = "libcgj-batch40237";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40237_id(void)
{
	return 40237u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40237 - report this TU's graph batch number.
 *
 * Always returns 40237.
 */
uint32_t
gj_batch_id_40237(void)
{
	(void)NULL;
	return b40237_id();
}

/*
 * gj_graph_milestone_40237 - report this TU's graph milestone revision.
 *
 * Always returns 40237. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_40237(void)
{
	return b40237_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_40237(void)
    __attribute__((alias("gj_batch_id_40237")));

uint32_t __gj_graph_milestone_40237(void)
    __attribute__((alias("gj_graph_milestone_40237")));
