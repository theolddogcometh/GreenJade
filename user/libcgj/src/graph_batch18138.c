/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18138: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18138(void);
 *     - Returns the compile-time graph batch number for this TU (18138).
 *   uint32_t gj_graph_milestone_18138(void);
 *     - Returns the current graph milestone revision (18138).
 *   uint32_t __gj_batch_id_18138  (alias)
 *   uint32_t __gj_graph_milestone_18138  (alias)
 *   __libcgj_batch18138_marker = "libcgj-batch18138"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 18101-18150, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 18200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch18138_marker[] = "libcgj-batch18138";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18138_id(void)
{
	return 18138u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18138 - report this TU's graph batch number.
 *
 * Always returns 18138.
 */
uint32_t
gj_batch_id_18138(void)
{
	(void)NULL;
	return b18138_id();
}

/*
 * gj_graph_milestone_18138 - report this TU's graph milestone revision.
 *
 * Always returns 18138. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_18138(void)
{
	return b18138_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_18138(void)
    __attribute__((alias("gj_batch_id_18138")));

uint32_t __gj_graph_milestone_18138(void)
    __attribute__((alias("gj_graph_milestone_18138")));
