/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2760: milestone 2760 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2760(void);
 *     - Returns the compile-time graph batch number for this TU (2760).
 *   uint32_t gj_graph_milestone_2760(void);
 *     - Returns the current graph milestone revision (2760).
 *   uint32_t __gj_batch_id_2760  (alias)
 *   uint32_t __gj_graph_milestone_2760  (alias)
 *   __libcgj_batch2760_marker = "libcgj-batch2760"
 *
 * Milestone for the post-2750 classic str predicate exclusive wave
 * (batches 2751-2759: str_length_u, str_nlength_u, str_equal_u,
 * str_neq_u, str_starts_u, str_ends_u, str_find_chr_u, str_rfind_chr_u,
 * str_span_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2760_marker[] = "libcgj-batch2760";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2760_id(void)
{
	return 2760u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2760 - report this TU's graph batch number.
 *
 * Always returns 2760.
 */
uint32_t
gj_batch_id_2760(void)
{
	(void)NULL;
	return b2760_id();
}

/*
 * gj_graph_milestone_2760 - report this TU's graph milestone revision.
 *
 * Always returns 2760 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2760(void)
{
	return b2760_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2760(void)
    __attribute__((alias("gj_batch_id_2760")));

uint32_t __gj_graph_milestone_2760(void)
    __attribute__((alias("gj_graph_milestone_2760")));
