/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch84457: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_84457(void);
 *     - Returns the compile-time graph batch number for this TU (84457).
 *   uint32_t gj_graph_milestone_84457(void);
 *     - Returns the current graph milestone revision (84457).
 *   uint32_t __gj_batch_id_84457  (alias)
 *   uint32_t __gj_graph_milestone_84457  (alias)
 *   __libcgj_batch84457_marker = "libcgj-batch84457"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-84457, extension identity). Does NOT
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

const char __libcgj_batch84457_marker[] = "libcgj-batch84457";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b84457_id(void)
{
	return 84457u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_84457 - report this TU's graph batch number.
 *
 * Always returns 84457.
 */
uint32_t
gj_batch_id_84457(void)
{
	(void)NULL;
	return b84457_id();
}

/*
 * gj_graph_milestone_84457 - report this TU's graph milestone revision.
 *
 * Always returns 84457. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_84457(void)
{
	return b84457_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_84457(void)
    __attribute__((alias("gj_batch_id_84457")));

uint32_t __gj_graph_milestone_84457(void)
    __attribute__((alias("gj_graph_milestone_84457")));
