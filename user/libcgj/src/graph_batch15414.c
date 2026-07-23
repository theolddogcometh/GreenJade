/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15414: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15414(void);
 *     - Returns the compile-time graph batch number for this TU (15414).
 *   uint32_t gj_graph_milestone_15414(void);
 *     - Returns the current graph milestone revision (15414).
 *   uint32_t __gj_batch_id_15414  (alias)
 *   uint32_t __gj_graph_milestone_15414  (alias)
 *   __libcgj_batch15414_marker = "libcgj-batch15414"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15401-15425). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Identity band for makefile_max target 15500. bar3 open.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch15414_marker[] = "libcgj-batch15414";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15414_id(void)
{
	return 15414u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15414 - report this TU's graph batch number.
 *
 * Always returns 15414.
 */
uint32_t
gj_batch_id_15414(void)
{
	(void)NULL;
	return b15414_id();
}

/*
 * gj_graph_milestone_15414 - report this TU's graph milestone revision.
 *
 * Always returns 15414. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_15414(void)
{
	return b15414_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_15414(void)
    __attribute__((alias("gj_batch_id_15414")));

uint32_t __gj_graph_milestone_15414(void)
    __attribute__((alias("gj_graph_milestone_15414")));
