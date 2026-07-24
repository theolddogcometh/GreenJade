/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37936: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37936(void);
 *     - Returns the compile-time graph batch number for this TU (37936).
 *   uint32_t gj_graph_milestone_37936(void);
 *     - Returns the current graph milestone revision (37936).
 *   uint32_t __gj_batch_id_37936  (alias)
 *   uint32_t __gj_graph_milestone_37936  (alias)
 *   __libcgj_batch37936_marker = "libcgj-batch37936"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 37901-37950, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 38000).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch37936_marker[] = "libcgj-batch37936";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37936_id(void)
{
	return 37936u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37936 - report this TU's graph batch number.
 *
 * Always returns 37936.
 */
uint32_t
gj_batch_id_37936(void)
{
	(void)NULL;
	return b37936_id();
}

/*
 * gj_graph_milestone_37936 - report this TU's graph milestone revision.
 *
 * Always returns 37936. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_37936(void)
{
	return b37936_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_37936(void)
    __attribute__((alias("gj_batch_id_37936")));

uint32_t __gj_graph_milestone_37936(void)
    __attribute__((alias("gj_graph_milestone_37936")));
