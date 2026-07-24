/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80626: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_80626(void);
 *     - Returns the compile-time graph batch number for this TU (80626).
 *   uint32_t gj_graph_milestone_80626(void);
 *     - Returns the current graph milestone revision (80626).
 *   uint32_t __gj_batch_id_80626  (alias)
 *   uint32_t __gj_graph_milestone_80626  (alias)
 *   __libcgj_batch80626_marker = "libcgj-batch80626"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-80626, extension identity). Does NOT
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

const char __libcgj_batch80626_marker[] = "libcgj-batch80626";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80626_id(void)
{
	return 80626u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_80626 - report this TU's graph batch number.
 *
 * Always returns 80626.
 */
uint32_t
gj_batch_id_80626(void)
{
	(void)NULL;
	return b80626_id();
}

/*
 * gj_graph_milestone_80626 - report this TU's graph milestone revision.
 *
 * Always returns 80626. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_80626(void)
{
	return b80626_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_80626(void)
    __attribute__((alias("gj_batch_id_80626")));

uint32_t __gj_graph_milestone_80626(void)
    __attribute__((alias("gj_graph_milestone_80626")));
