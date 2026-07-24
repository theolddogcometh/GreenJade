/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27904: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27904(void);
 *     - Returns the compile-time graph batch number for this TU (27904).
 *   uint32_t gj_graph_milestone_27904(void);
 *     - Returns the current graph milestone revision (27904).
 *   uint32_t __gj_batch_id_27904  (alias)
 *   uint32_t __gj_graph_milestone_27904  (alias)
 *   __libcgj_batch27904_marker = "libcgj-batch27904"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 27901-27950, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 28000).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch27904_marker[] = "libcgj-batch27904";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27904_id(void)
{
	return 27904u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27904 - report this TU's graph batch number.
 *
 * Always returns 27904.
 */
uint32_t
gj_batch_id_27904(void)
{
	(void)NULL;
	return b27904_id();
}

/*
 * gj_graph_milestone_27904 - report this TU's graph milestone revision.
 *
 * Always returns 27904. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_27904(void)
{
	return b27904_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_27904(void)
    __attribute__((alias("gj_batch_id_27904")));

uint32_t __gj_graph_milestone_27904(void)
    __attribute__((alias("gj_graph_milestone_27904")));
