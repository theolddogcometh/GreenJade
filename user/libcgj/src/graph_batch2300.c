/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2300: milestone 2300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2300(void);
 *     - Returns the compile-time graph batch number for this TU (2300).
 *   uint32_t gj_graph_milestone_2300(void);
 *     - Returns the current graph milestone revision (2300).
 *   uint32_t __gj_batch_id_2300  (alias)
 *   uint32_t __gj_graph_milestone_2300  (alias)
 *   __libcgj_batch2300_marker = "libcgj-batch2300"
 *
 * Milestone for the exclusive product helpers wave (batches 2291-2299:
 * wave_id_2300, export_band_ok, batch_gap_p, continuum_span_ok,
 * shell_ver_ok_hint, smoke_band_ok, soname_major_ok, license_dual_p,
 * bar3_ready_hint). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2300_marker[] = "libcgj-batch2300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2300_id(void)
{
	return 2300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2300 - report this TU's graph batch number.
 *
 * Always returns 2300.
 */
uint32_t
gj_batch_id_2300(void)
{
	(void)NULL;
	return b2300_id();
}

/*
 * gj_graph_milestone_2300 - report this TU's graph milestone revision.
 *
 * Always returns 2300 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2300(void)
{
	return b2300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2300(void)
    __attribute__((alias("gj_batch_id_2300")));

uint32_t __gj_graph_milestone_2300(void)
    __attribute__((alias("gj_graph_milestone_2300")));
