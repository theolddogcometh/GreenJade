/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2520: milestone 2520 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2520(void);
 *     - Returns the compile-time graph batch number for this TU (2520).
 *   uint32_t gj_graph_milestone_2520(void);
 *     - Returns the current graph milestone revision (2520).
 *   uint32_t __gj_batch_id_2520  (alias)
 *   uint32_t __gj_graph_milestone_2520  (alias)
 *   __libcgj_batch2520_marker = "libcgj-batch2520"
 *
 * Milestone for the JSON exclusive lightweight wave (batches 2511-2519:
 * escape_needed_p, escape_len_u, unescape_len_u, is_number_start,
 * is_ident_start, skip_string_n, skip_number_n, depth_delta,
 * is_structural). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2520_marker[] = "libcgj-batch2520";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2520_id(void)
{
	return 2520u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2520 - report this TU's graph batch number.
 *
 * Always returns 2520.
 */
uint32_t
gj_batch_id_2520(void)
{
	(void)NULL;
	return b2520_id();
}

/*
 * gj_graph_milestone_2520 - report this TU's graph milestone revision.
 *
 * Always returns 2520 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2520(void)
{
	return b2520_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2520(void)
    __attribute__((alias("gj_batch_id_2520")));

uint32_t __gj_graph_milestone_2520(void)
    __attribute__((alias("gj_graph_milestone_2520")));
