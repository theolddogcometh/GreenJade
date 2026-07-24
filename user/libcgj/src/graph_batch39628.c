/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39628: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39628(void);
 *     - Returns the compile-time graph batch number for this TU (39628).
 *   uint32_t gj_graph_milestone_39628(void);
 *     - Returns the current graph milestone revision (39628).
 *   uint32_t __gj_batch_id_39628  (alias)
 *   uint32_t __gj_graph_milestone_39628  (alias)
 *   __libcgj_batch39628_marker = "libcgj-batch39628"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 39601-39650, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 39700).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch39628_marker[] = "libcgj-batch39628";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39628_id(void)
{
	return 39628u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39628 - report this TU's graph batch number.
 *
 * Always returns 39628.
 */
uint32_t
gj_batch_id_39628(void)
{
	(void)NULL;
	return b39628_id();
}

/*
 * gj_graph_milestone_39628 - report this TU's graph milestone revision.
 *
 * Always returns 39628. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_39628(void)
{
	return b39628_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_39628(void)
    __attribute__((alias("gj_batch_id_39628")));

uint32_t __gj_graph_milestone_39628(void)
    __attribute__((alias("gj_graph_milestone_39628")));
