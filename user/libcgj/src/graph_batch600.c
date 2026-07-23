/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch600: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_600(void);
 *     — Returns the compile-time graph batch number for this TU (600).
 *   uint32_t gj_graph_milestone_600(void);
 *     — Returns the current graph milestone revision (600).
 *   uint32_t __gj_batch_id_600  (alias)
 *   uint32_t __gj_graph_milestone_600  (alias)
 *   __libcgj_batch600_marker = "libcgj-batch600"
 *
 * Does NOT define gj_batch_id (batch500) or gj_graph_milestone /
 * gj_batch_id_550 (batch550) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch600_marker[] = "libcgj-batch600";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_600 — report this TU's graph batch number.
 *
 * Always returns 600. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_600(void)
{
	return 600u;
}

/*
 * gj_graph_milestone_600 — report this TU's graph milestone revision.
 *
 * Always returns 600 (this milestone TU). Distinct from gj_batch_id
 * (batch500) and gj_graph_milestone (batch550) so all can coexist at
 * link time.
 */
uint32_t
gj_graph_milestone_600(void)
{
	return 600u;
}

uint32_t __gj_batch_id_600(void)
    __attribute__((alias("gj_batch_id_600")));

uint32_t __gj_graph_milestone_600(void)
    __attribute__((alias("gj_graph_milestone_600")));
