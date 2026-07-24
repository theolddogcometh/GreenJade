/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch60567: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_60567(void);
 *     - Returns the compile-time graph batch number for this TU (60567).
 *   uint32_t gj_graph_milestone_60567(void);
 *     - Returns the current graph milestone revision (60567).
 *   uint32_t __gj_batch_id_60567  (alias)
 *   uint32_t __gj_graph_milestone_60567  (alias)
 *   __libcgj_batch60567_marker = "libcgj-batch60567"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 25801-60567, extension identity). Does NOT
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

const char __libcgj_batch60567_marker[] = "libcgj-batch60567";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b60567_id(void)
{
	return 60567u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_60567 - report this TU's graph batch number.
 *
 * Always returns 60567.
 */
uint32_t
gj_batch_id_60567(void)
{
	(void)NULL;
	return b60567_id();
}

/*
 * gj_graph_milestone_60567 - report this TU's graph milestone revision.
 *
 * Always returns 60567. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_60567(void)
{
	return b60567_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_60567(void)
    __attribute__((alias("gj_batch_id_60567")));

uint32_t __gj_graph_milestone_60567(void)
    __attribute__((alias("gj_graph_milestone_60567")));
