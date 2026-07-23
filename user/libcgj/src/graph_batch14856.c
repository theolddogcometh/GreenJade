/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14856: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14856(void);
 *     - Returns the compile-time graph batch number for this TU (14856).
 *   uint32_t gj_graph_milestone_14856(void);
 *     - Returns the current graph milestone revision (14856).
 *   uint32_t __gj_batch_id_14856  (alias)
 *   uint32_t __gj_graph_milestone_14856  (alias)
 *   __libcgj_batch14856_marker = "libcgj-batch14856"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 14851-14875). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14856_marker[] = "libcgj-batch14856";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14856_id(void)
{
	return 14856u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14856 - report this TU's graph batch number.
 *
 * Always returns 14856.
 */
uint32_t
gj_batch_id_14856(void)
{
	(void)NULL;
	return b14856_id();
}

/*
 * gj_graph_milestone_14856 - report this TU's graph milestone revision.
 *
 * Always returns 14856. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14856(void)
{
	return b14856_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14856(void)
    __attribute__((alias("gj_batch_id_14856")));

uint32_t __gj_graph_milestone_14856(void)
    __attribute__((alias("gj_graph_milestone_14856")));
