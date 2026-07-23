/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15013: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15013(void);
 *     - Returns the compile-time graph batch number for this TU (15013).
 *   uint32_t gj_graph_milestone_15013(void);
 *     - Returns the current graph milestone revision (15013).
 *   uint32_t __gj_batch_id_15013  (alias)
 *   uint32_t __gj_graph_milestone_15013  (alias)
 *   __libcgj_batch15013_marker = "libcgj-batch15013"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15001-15025 post-M=15000
 * continuum fill). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15013_marker[] = "libcgj-batch15013";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15013_id(void)
{
	return 15013u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15013 - report this TU's graph batch number.
 *
 * Always returns 15013.
 */
uint32_t
gj_batch_id_15013(void)
{
	(void)NULL;
	return b15013_id();
}

/*
 * gj_graph_milestone_15013 - report this TU's graph milestone revision.
 *
 * Always returns 15013. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15013(void)
{
	return b15013_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15013(void)
    __attribute__((alias("gj_batch_id_15013")));

uint32_t __gj_graph_milestone_15013(void)
    __attribute__((alias("gj_graph_milestone_15013")));
