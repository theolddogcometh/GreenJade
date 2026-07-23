/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2110: milestone 2110 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2110(void);
 *     - Returns the compile-time graph batch number for this TU (2110).
 *   uint32_t gj_graph_milestone_2110(void);
 *     - Returns the current graph milestone revision (2110).
 *   uint32_t __gj_batch_id_2110  (alias)
 *   uint32_t __gj_graph_milestone_2110  (alias)
 *   __libcgj_batch2110_marker = "libcgj-batch2110"
 *
 * Milestone for the post-2100 str trim exclusive wave (batches 2101-2109:
 * str_ltrim_ascii_span, str_rtrim_ascii_span, str_trim_ascii_span,
 * str_collapse_ws_len, str_has_ws_p, str_has_ctrl_p,
 * str_printable_ascii_p, str_width_hint_n, str_tab_expand_len). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2110_marker[] = "libcgj-batch2110";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2110_id(void)
{
	return 2110u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2110 - report this TU's graph batch number.
 *
 * Always returns 2110.
 */
uint32_t
gj_batch_id_2110(void)
{
	(void)NULL;
	return b2110_id();
}

/*
 * gj_graph_milestone_2110 - report this TU's graph milestone revision.
 *
 * Always returns 2110 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2110(void)
{
	return b2110_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2110(void)
    __attribute__((alias("gj_batch_id_2110")));

uint32_t __gj_graph_milestone_2110(void)
    __attribute__((alias("gj_graph_milestone_2110")));
