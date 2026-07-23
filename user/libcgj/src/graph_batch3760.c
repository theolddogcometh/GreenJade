/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3760: gj_batch_id_3760 - batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3760(void);
 *     - Returns the compile-time graph batch number for this TU (3760).
 *   uint32_t __gj_batch_id_3760  (alias)
 *   __libcgj_batch3760_marker = "libcgj-batch3760"
 *
 * Graph exclusive continuum CREATE-ONLY (3751-3760): adj_test/set/clr,
 * degree, edge_count, bfs/dfs visit mark, topo_ready, scc_link_min.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3760_marker[] = "libcgj-batch3760";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3760_id(void)
{
	return 3760u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3760 - report this TU's graph batch number.
 *
 * Always returns 3760.
 */
uint32_t
gj_batch_id_3760(void)
{
	(void)NULL;
	return b3760_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3760(void)
    __attribute__((alias("gj_batch_id_3760")));
