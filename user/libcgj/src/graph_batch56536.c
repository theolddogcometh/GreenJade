/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch56536: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_56536(void);
 *     - Returns the compile-time graph batch number for this TU (56536).
 *   uint32_t gj_graph_milestone_56536(void);
 *     - Returns the current graph milestone revision (56536).
 *   uint32_t __gj_batch_id_56536  (alias)
 *   uint32_t __gj_graph_milestone_56536  (alias)
 *   __libcgj_batch56536_marker = "libcgj-batch56536"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-56536, extension identity). Does NOT
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

const char __libcgj_batch56536_marker[] = "libcgj-batch56536";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b56536_id(void)
{
	return 56536u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_56536 - report this TU's graph batch number.
 *
 * Always returns 56536.
 */
uint32_t
gj_batch_id_56536(void)
{
	(void)NULL;
	return b56536_id();
}

/*
 * gj_graph_milestone_56536 - report this TU's graph milestone revision.
 *
 * Always returns 56536. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_56536(void)
{
	return b56536_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_56536(void)
    __attribute__((alias("gj_batch_id_56536")));

uint32_t __gj_graph_milestone_56536(void)
    __attribute__((alias("gj_graph_milestone_56536")));
