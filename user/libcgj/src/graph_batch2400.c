/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2400: milestone 2400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2400(void);
 *     - Returns the compile-time graph batch number for this TU (2400).
 *   uint32_t gj_graph_milestone_2400(void);
 *     - Returns the current graph milestone revision (2400).
 *   uint32_t __gj_batch_id_2400  (alias)
 *   uint32_t __gj_graph_milestone_2400  (alias)
 *   __libcgj_batch2400_marker = "libcgj-batch2400"
 *
 * Milestone for the exclusive product helpers wave (batches 2391-2399:
 * wave_id_2400, export_floor_2400, batch_span_ok_u, smoke_green_p,
 * smoke_red_p, dyn_smoke_ok_p, shell_continuum_ok, product_bar3_bits,
 * libcgj_soname_ok_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2400_marker[] = "libcgj-batch2400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2400_id(void)
{
	return 2400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2400 - report this TU's graph batch number.
 *
 * Always returns 2400.
 */
uint32_t
gj_batch_id_2400(void)
{
	(void)NULL;
	return b2400_id();
}

/*
 * gj_graph_milestone_2400 - report this TU's graph milestone revision.
 *
 * Always returns 2400 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2400(void)
{
	return b2400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2400(void)
    __attribute__((alias("gj_batch_id_2400")));

uint32_t __gj_graph_milestone_2400(void)
    __attribute__((alias("gj_graph_milestone_2400")));
