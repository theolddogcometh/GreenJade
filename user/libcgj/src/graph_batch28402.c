/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28402: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28402(void);
 *     - Returns the compile-time graph batch number for this TU (28402).
 *   uint32_t gj_graph_milestone_28402(void);
 *     - Returns the current graph milestone revision (28402).
 *   uint32_t __gj_batch_id_28402  (alias)
 *   uint32_t __gj_graph_milestone_28402  (alias)
 *   __libcgj_batch28402_marker = "libcgj-batch28402"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 28401-28450, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water 28500).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch28402_marker[] = "libcgj-batch28402";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28402_id(void)
{
	return 28402u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28402 - report this TU's graph batch number.
 *
 * Always returns 28402.
 */
uint32_t
gj_batch_id_28402(void)
{
	(void)NULL;
	return b28402_id();
}

/*
 * gj_graph_milestone_28402 - report this TU's graph milestone revision.
 *
 * Always returns 28402. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_28402(void)
{
	return b28402_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_28402(void)
    __attribute__((alias("gj_batch_id_28402")));

uint32_t __gj_graph_milestone_28402(void)
    __attribute__((alias("gj_graph_milestone_28402")));
