/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch700: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_700(void);
 *     — Returns the compile-time graph batch number for this TU (700).
 *   uint32_t gj_graph_milestone_700(void);
 *     — Returns the current graph milestone revision (700).
 *   uint32_t __gj_batch_id_700  (alias)
 *   uint32_t __gj_graph_milestone_700  (alias)
 *   __libcgj_batch700_marker = "libcgj-batch700"
 *
 * Does NOT define gj_batch_id (batch500), gj_graph_milestone /
 * gj_batch_id_550 (batch550), gj_batch_id_600 / gj_graph_milestone_600
 * (batch600), gj_batch_id_630 (batch630), gj_batch_id_650 (batch650),
 * or gj_batch_id_670 (batch670) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch700_marker[] = "libcgj-batch700";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_700 — report this TU's graph batch number.
 *
 * Always returns 700. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_700(void)
{
	return 700u;
}

/*
 * gj_graph_milestone_700 — report this TU's graph milestone revision.
 *
 * Always returns 700 (this milestone TU). Distinct from gj_batch_id
 * (batch500), gj_graph_milestone (batch550), and gj_graph_milestone_600
 * (batch600) so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_700(void)
{
	return 700u;
}

uint32_t __gj_batch_id_700(void)
    __attribute__((alias("gj_batch_id_700")));

uint32_t __gj_graph_milestone_700(void)
    __attribute__((alias("gj_graph_milestone_700")));
