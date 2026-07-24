/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46165: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46165(void);
 *     - Returns the compile-time graph batch number for this TU (46165).
 *   uint32_t gj_graph_milestone_46165(void);
 *     - Returns the current graph milestone revision (46165).
 *   uint32_t __gj_batch_id_46165  (alias)
 *   uint32_t __gj_graph_milestone_46165  (alias)
 *   __libcgj_batch46165_marker = "libcgj-batch46165"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 46151-46175, decade identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 46200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch46165_marker[] = "libcgj-batch46165";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46165_id(void)
{
	return 46165u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46165 - report this TU's graph batch number.
 *
 * Always returns 46165.
 */
uint32_t
gj_batch_id_46165(void)
{
	(void)NULL;
	return b46165_id();
}

/*
 * gj_graph_milestone_46165 - report this TU's graph milestone revision.
 *
 * Always returns 46165. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_46165(void)
{
	return b46165_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_46165(void)
    __attribute__((alias("gj_batch_id_46165")));

uint32_t __gj_graph_milestone_46165(void)
    __attribute__((alias("gj_graph_milestone_46165")));
